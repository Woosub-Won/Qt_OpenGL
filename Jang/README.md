# 기본
## 파일 이름 양식
~~{챕터}-{장}-{내용}~~
파일이 너무 많고 관리가 힘들기 때문에 메인 파일들 하나를 사용하고 커밋을 통해 장을 완료할 때 마다 커밋


# GLSL 4.0 학습 목차

## Chapter 1: Getting Started with GLSL 4.0
1. **Introduction**
2. **Using the GLEW Library to access the latest OpenGL functionality**
3. **Using the GLM library for mathematics**
4. **Determining the GLSL and OpenGL version**
5. **Compiling a shader**
6. **Linking a shader program**
7. **Sending data to a shader using per-vertex attributes and vertex buffer objects**
8. **Getting a list of active vertex input attributes and indices**
9. **Sending data to a shader using uniform variables**
10. **Getting a list of active uniform variables**
11. **Using uniform blocks and uniform buffer objects**
12. **Building a C++ shader program class**

## Chapter 2: The Basics of GLSL Shaders
1. **Introduction**
2. **Implementing diffuse, per-vertex shading with a single point light source**
3. **Implementing per-vertex ambient, diffuse, and specular (ADS) shading**
4. **Using functions in shaders**
5. **Implementing two-sided shading**
6. **Implementing flat shading**
7. **Using subroutines to select shader functionality**
8. **Discarding fragments to create a perforated look**

## Chapter 3: Lighting, Shading Effects, and Optimizations
1. **Introduction**
2. **Shading with multiple positional lights**
3. **Shading with a directional light source**
4. **Using per-fragment shading for improved realism**
5. **Using the halfway vector for improved performance**
6. **Simulating a spotlight**
7. **Creating a cartoon shading effect**
8. **Simulating fog**

## Chapter 4: Using Textures
1. **Introduction**
2. **Applying a 2D texture**
3. **Applying multiple textures**
4. **Using alpha maps to discard pixels**
5. **Using normal maps**
6. **Simulating reflection with cube maps**
7. **Simulating refraction with cube maps**
8. **Image-based lighting**
9. **Applying a projected texture**
10. **Rendering to a texture**

## Chapter 5: Image Processing and Screen Space Techniques
1. **Introduction**
2. **Applying an edge detection filter**
3. **Applying a Gaussian blur filter**
4. **Creating a "bloom" effect**
5. **Using gamma correction to improve image quality**
6. **Using multisample anti-aliasing**
7. **Using deferred shading**

## Chapter 6: Using Geometry and Tessellation Shaders
1. **Introduction**
2. **Point sprites with the geometry shader**
3. **Drawing a wireframe on top of a shaded mesh**
4. **Drawing silhouette lines using the geometry shader**
5. **Tessellating a curve**
6. **Tessellating a 2D quad**
7. **Tessellating a 3D surface**
8. **Tessellating based on depth**

## Chapter 7: Shadows
1. **Introduction**
2. **Rendering shadows with shadow maps**
3. **Anti-aliasing shadow edges with PCF**
4. **Creating soft shadow edges with random sampling**
5. **Improving realism with prebaked ambient occlusion**

## Chapter 8: Using Noise in Shaders
1. **Introduction**
2. **Creating a noise texture using libnoise**
3. **Creating a seamless noise texture**
4. **Creating a cloud-like effect**
5. **Creating a wood grain effect**
6. **Creating a disintegration effect**
7. **Creating a paint-spatter effect**
8. **Creating a night-vision effect**

## Chapter 9: Animation and Particles
1. **Introduction**
2. **Animating a surface with vertex displacement**
3. **Creating a particle fountain**
4. **Creating a particle system using transform feedback**
5. **Creating a particle system using instanced particles**
6. **Simulating fire with particles**
7. **Simulating smoke with particles**
