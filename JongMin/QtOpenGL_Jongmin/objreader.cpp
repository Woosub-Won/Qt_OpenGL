#include "objreader.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

objReader::objReader() {}

bool objReader::load(const QString& filePath) {
    vertexBuffer.clear();  // 이전 데이터 초기화
    return parseOBJ(filePath, vertexBuffer);
}

const QVector<VertexData>& objReader::getVertexData() const {
    return vertexBuffer;
}
bool objReader::parseOBJ(const QString& filePath, QVector<VertexData>& vertexBuffer) {
    QVector<QVector3D> positions;
    QVector<QVector3D> normals;  // 정점별 노말 계산용
    QVector<QVector3D> faceNormals;  // 면별 노말 저장용
    QVector<QVector2D> texCoords;  // 정점별 텍스처 좌표 계산용

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Failed to open OBJ file: %s", qPrintable(filePath));
        return false;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(" ", Qt::SkipEmptyParts);
        if (parts.isEmpty()) continue;

        if (parts[0] == "v") {
            QVector3D position(parts[1].toFloat(), parts[2].toFloat(), parts[3].toFloat());
            positions.append(position);

            // 기본 텍스처 좌표 계산 (평면 매핑)
            float u = position.x();
            float v = position.z();
            texCoords.append(QVector2D(u, v));

            // 초기화된 노말
            normals.append(QVector3D(0.0f, 0.0f, 0.0f));
        } else if (parts[0] == "f") {
            QVector<int> indices;
            for (int i = 1; i < parts.size(); ++i) {
                int posIdx = parts[i].toInt() - 1;  // OBJ 인덱스는 1부터 시작
                if (posIdx < 0 || posIdx >= positions.size()) {
                    qWarning("Invalid position index: %d", posIdx);
                    continue;
                }
                indices.append(posIdx);
            }

            // 삼각형의 면 노말 계산
            if (indices.size() == 3) {  // 삼각형만 처리
                QVector3D v0 = positions[indices[0]];
                QVector3D v1 = positions[indices[1]];
                QVector3D v2 = positions[indices[2]];

                QVector3D edge1 = v1 - v0;
                QVector3D edge2 = v2 - v0;
                QVector3D faceNormal = QVector3D::crossProduct(edge1, edge2).normalized();
                faceNormals.append(faceNormal);

                // 각 정점에 면 노말 추가 (평균화를 위해 더함)
                normals[indices[0]] += faceNormal;
                normals[indices[1]] += faceNormal;
                normals[indices[2]] += faceNormal;
            }
        }
    }

    file.close();

    // 정점별로 노말 벡터 정규화
    for (int i = 0; i < normals.size(); ++i) {
        normals[i] = normals[i].normalized();
    }

    // 최종 버퍼 생성
    for (int i = 0; i < positions.size(); ++i) {
        vertexBuffer.append({positions[i], texCoords[i], normals[i]});
    }

    return true;
}
