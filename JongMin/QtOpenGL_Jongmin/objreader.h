#ifndef OBJREADER_H
#define OBJREADER_H

#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QString>

struct VertexData {
    QVector3D position;
    QVector2D texCoord;
    QVector3D normal;
};

class objReader {
public:
    objReader();

    // OBJ 파일 로드 메서드
    bool load(const QString& filePath);

    // 로드된 정점 데이터를 반환
    const QVector<VertexData>& getVertexData() const;

private:
    // 내부 데이터
    QVector<VertexData> vertexBuffer;

    // OBJ 파일 파싱 메서드
    bool parseOBJ(const QString& filePath, QVector<VertexData>& vertexBuffer);
};

#endif // OBJREADER_H
