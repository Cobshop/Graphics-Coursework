#include "HeightMap.h"
#include <iostream>

HeightMap::HeightMap(const std::string& name) {
	int  iWidth , iHeight , iChans;
	unsigned char* data = SOIL_load_image(name.c_str(), &iWidth , &iHeight , &iChans , 1);
	if (!data) {
		std::cout  << "Heightmap  can?t load  file!\n";
		return;
	}

	numVertices = iWidth * iHeight;
	numIndices = (iWidth - 1) * (iHeight - 1) * 6;
	vertices = new  Vector3[numVertices];
	textureCoords = new  Vector2[numVertices];
	indices = new  GLuint[numIndices];
	colours = new Vector4[numVertices];

	Vector3 vertexScale = Vector3(64.0f, 5.0f, 64.0f);
	Vector2 textureScale = Vector2(0.1, 0.1); //Vector2(0.2, 0.2);
	
	for (int z = 0; z < iHeight; ++z) {
		for (int x = 0; x < iWidth; ++x) {
			int  offset = (z * iWidth) + x;
			vertices[offset] = Vector3(x, data[offset], z) * vertexScale;
			textureCoords[offset] = Vector2(x, z) * textureScale;
			/*if(data[offset] >= 1)
				colours[offset] = Vector4(data[offset]/100, data[offset]/100, data[offset]/100, 1.0f);
			else
				colours[offset] = Vector4(0, 0, 0, 1.0f);*/
		}
	}
	SOIL_free_image_data(data);

	int i = 0;
	
	for (int z = 0; z < iHeight - 1; ++z) {
		for (int x = 0; x < iWidth - 1; ++x) {
			int a = (z * (iWidth)) + x;
			int b = (z * (iWidth)) + (x + 1);
			int c = ((z + 1) * (iWidth)) + (x + 1);
			int d = ((z + 1) * (iWidth)) + x;
			
			indices[i++] = a;
			indices[i++] = c;
			indices[i++] = b;
			
			indices[i++] = c;
			indices[i++] = a;
			indices[i++] = d;
		}
	}
	GenerateNormals();
	GenerateTangents();

	BufferData();
	
	heightmapSize.x = vertexScale.x * (iWidth - 1);
	heightmapSize.y = vertexScale.y * 255.0f;
	heightmapSize.z = vertexScale.z * (iHeight  - 1);
}