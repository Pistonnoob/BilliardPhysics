#include "Model.h"

Model::Model()
{
	this->vertexBuffer = nullptr;
	this->indexBuffer = nullptr;
}

Model::~Model()
{
}

bool Model::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::string objFilename)
{
	std::vector<Vertex>* vertices;
	unsigned long* indices = nullptr;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;
	HRESULT hresult;
	bool result;

	//Create the vertex array
	vertices = new std::vector<Vertex>;
	if (!vertices) {
		return false;
	}

	if (objFilename == "") {
		//Set the number of vertices in the vertex array
		this->vertexCount = 3;
		//Set the numer of indices in the index array
		this->indexCount = 3;

		Vertex tempVertex;

		//Create the index array
		indices = new unsigned long[this->indexCount];
		if (!indices) {
			return false;
		}

		//Load the vertex array with data
		//Order is important, otherwise the triangle will be facing the opposite direction
		tempVertex.position = DirectX::XMFLOAT3(-1.0, -1.0f, 0.0f); //Bottom left
		tempVertex.texture = DirectX::XMFLOAT2(0.0f, 1.0f);
		tempVertex.normal = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertices->push_back(tempVertex);

		tempVertex.position = DirectX::XMFLOAT3(0.0, 1.0f, 0.0f); //Top Middle
		tempVertex.texture = DirectX::XMFLOAT2(0.5f, 0.0f);
		tempVertex.normal = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertices->push_back(tempVertex);

		tempVertex.position = DirectX::XMFLOAT3(1.0, -1.0f, 0.0f); //Bottom right
		tempVertex.texture = DirectX::XMFLOAT2(1.0f, 1.0f);
		tempVertex.normal = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertices->push_back(tempVertex);

		//Load the index array with data
		indices[0] = 0;	//Bottom left
		indices[1] = 1;	//Top Middle
		indices[2] = 2;	//Bottom right
	}
	else {
		std::string materialLib;
		this->LoadObj(objFilename.c_str(), vertices, indices, materialLib);
		this->texture = new Texture;
		if (!this->texture->Initialize(device, deviceContext, materialLib)) {
			return false;
		}

 		for (int i = 0; i < this->materialNames.size(); i++) {
			this->materialIndices.push_back(this->texture->GetMaterialIndexFromName(this->materialNames.at(i)));
		}
	}

	//Set the description of the static vertex buffer
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * this->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//Give the subresource structure a pointer to the vertex data
	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = &((*vertices)[0]);
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//Create the vertex buffer
 	hresult = device->CreateBuffer(&vertexBufferDesc, &vertexData, &this->vertexBuffer);
	if (FAILED(hresult)) {
		return false;
	}

	//Set up the description of the static index buffer
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * this->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//Create the index buffer
	hresult = device->CreateBuffer(&indexBufferDesc, &indexData, &this->indexBuffer);
	if (FAILED(hresult)) {
		return false;
	}

	delete vertices;
	delete[] indices;

	this->worldMatrix = DirectX::XMMatrixIdentity();
}

void Model::Render(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned offset;

	//Set vertex buffer stride and offset
	stride = sizeof(Vertex);
	offset = 0;

	//Set the vertex buffer to active in the input assembly so it can rendered
	deviceContext->IASetVertexBuffers(0, 1, &this->vertexBuffer, &stride, &offset);

	//Set the index buffer to active in the input assembler so it can be rendered
	deviceContext->IASetIndexBuffer(this->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the type od primitiv that should be rendered from this vertex buffer, in this case triangles
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

void Model::Shutdown()
{
	if (this->indexBuffer) {
		this->indexBuffer->Release();
		this->indexBuffer = nullptr;
	}
	if (this->vertexBuffer) {
		this->vertexBuffer->Release();
		this->vertexBuffer = nullptr;
	}
	if (this->texture) {
		this->texture->Shutdown();
		delete this->texture;
		this->texture = nullptr;
	}
}

int Model::GetNrOfSubsets()
{
	return this->subsetIndices.size();
}

void Model::GetDeferredShaderParameters(DeferredShaderParameters* params, int subsetIndex, int& indexCount, int& indexStart)
{
	indexStart = this->subsetIndices.at(subsetIndex);
	if (subsetIndex + 1 < this->subsetIndices.size()) {
		indexCount = this->subsetIndices.at(subsetIndex + 1) - indexStart;
	}
	else {
		indexCount = this->indexCount - indexStart;
	}

	if (params) {
		Texture::Material tempMat = this->texture->GetMaterial(this->materialIndices.at(subsetIndex));

		params->diffColor = tempMat.diffColor;
		params->ambientColor = tempMat.ambientColor;
		params->specColor = tempMat.specColor;

		params->worldMatrix = this->worldMatrix;

		if (tempMat.hasTexture) {
			params->diffTexture = this->texture->GetTexture(tempMat.textureIndex);
		}
		else {
			params->diffTexture = NULL;
		}
	}
}

void Model::SetWorldMatrix(DirectX::XMMATRIX worldMatrix)
{
	this->worldMatrix = worldMatrix;
}

void Model::GetWorldMatrix(DirectX::XMMATRIX& worldMatrix)
{
	worldMatrix = this->worldMatrix;
}

void Model::SetColor(DirectX::XMFLOAT3 newColor)
{
	this->color = newColor;
}

ID3D11ShaderResourceView* Model::SwapTextureView(ID3D11ShaderResourceView* textureView, int textureIndex)
{
	return this->texture->SwapTextureView(textureView, textureIndex);
}

int Model::GetVertexCount()
{
	return this->vertexCount;
}

bool Model::LoadObj(const char* filename, std::vector<Vertex>* outputVertices, unsigned long*& outputIndices, std::string& materialLib)
{
 	DirectX::XMFLOAT3 tempVertex;
	DirectX::XMFLOAT2 tempUV;
	DirectX::XMFLOAT3 tempNormal;
	unsigned int vertexIndex[3];
	unsigned int uvIndex[3];
	unsigned int normalIndex[3];
	std::vector<DirectX::XMFLOAT3> tempVertices;
	std::vector<DirectX::XMFLOAT2> tempUvs;
	std::vector<DirectX::XMFLOAT3> tempNormals;
	std::vector<unsigned int> vertexIndices;
	std::vector<unsigned int> uvIndices;
	std::vector<unsigned int> normalIndices;
	std::string line;
	std::string junks;
	std::string tempLine;
	double point[3];
	char junk;
	std::stringstream ss;
	std::fstream file;
	bool newGroup = false;
	int tempSubset = 0;
	std::string path = "..\\SmallGameProject\\Resources\\OBJ\\";
	path.append(filename);
	path.append(".ace");
	file.open(path, std::ios::in);
 	if (file.is_open()) { //Model has been loaded before and therefor has binary files to read
		std::getline(file, line);
		ss.str(line);
		ss >> this->vertexCount >> this->indexCount; //Read the sizes

		std::getline(file, line);
		ss.clear();
		ss.str(line);
		ss >> materialLib; //Read the .mtl filename

		while (std::getline(file, line)) { //Read all the subset indices and corresponding materials
			ss.clear();
			ss.str(line);
			ss >> tempSubset >> tempLine;
			subsetIndices.push_back(tempSubset);
			materialNames.push_back(tempLine);
		}
		ss.clear();
		file.close();
		path = "..\\SmallGameProject\\Resources\\OBJ\\";
		path.append(filename);
		path.append("V.bin");
		file.open(path, std::ios::binary | std::ios::in);
		if (!file.is_open()) {
			return false;
		}
		Vertex* tempVerticesArray = new Vertex[this->vertexCount];

		file.read((char*)tempVerticesArray, sizeof(Vertex) * this->vertexCount); //Read the binary file containing the vertex data and save for output
		file.close();

		outputVertices->insert(outputVertices->end(), &tempVerticesArray[0], &tempVerticesArray[this->vertexCount]);
		this->vertPositions.insert(this->vertPositions.end(), &tempVerticesArray[0].position, &tempVerticesArray[this->vertexCount].position);

		delete[] tempVerticesArray;

		path = "..\\SmallGameProject\\Resources\\OBJ\\";
		path.append(filename);
		path.append("I.bin");
		file.open(path, std::ios::binary | std::ios::in);
		if (!file.is_open()) {
			return false;
		}

		outputIndices = new unsigned long[this->indexCount];
		file.read((char*)outputIndices, sizeof(unsigned long) * this->indexCount);//Read the binary file containing the index data and save for output
		file.close();
	}
	else { //If it's the first time models is being loaded read and parse the obj file
		path = "..\\SmallGameProject\\Resources\\OBJ\\";
		path.append(filename);
		path.append(".obj");
		file.open(path, std::ios::in);
		if (!file.is_open()) {
			return false;
		}

		while (std::getline(file, line)) {
			if (line.size() > 0) {
				if (line.at(0) == 'v') {
					if (line.at(1) == ' ') { //Save vertex position coordinates
						ss.clear();
						ss.str(line);
						ss >> junk >> tempVertex.x >> tempVertex.y >> tempVertex.z;

						tempVertices.push_back(tempVertex);
					}
					else if (line.at(1) == 't') { //Save vertex texture coordiantes
						ss.clear();
						ss.str(line);
						ss >> junks >> tempUV.x >> tempUV.y;
						tempUV.y = 1.0f - tempUV.y;
						tempUvs.push_back(tempUV);
					}
					else if (line.at(1) == 'n') { //Save the vertex normals
						ss.clear();
						ss.str(line);
						ss >> junks >> tempNormal.x >> tempNormal.y >> tempNormal.z;
						tempNormals.push_back(tempNormal);
					}
				}
				else if (line.at(0) == 'g') { //Create a new subset
					this->subsetIndices.push_back(vertexIndices.size());
					newGroup = true;
				}
				else if (line.substr(0, 6) == "usemtl") { //Subset uses this material
					ss.clear();
					ss.str(line);
					ss >> junks >> tempLine;
					this->materialNames.push_back(tempLine);
					if (!newGroup) {
						this->subsetIndices.push_back(vertexIndices.size());
					}
					else {
						newGroup = false;
					}
				}
				else if (line.at(0) == 'f') { //Save the vertices order from faces
					if (newGroup) {
						this->materialNames.push_back(this->materialNames.back());
						newGroup = false;
					}
					ss.clear();
					ss.str(line);
					ss >> junk >> vertexIndex[0] >> junk >> uvIndex[0] >> junk >> normalIndex[0]
						>> vertexIndex[1] >> junk >> uvIndex[1] >> junk >> normalIndex[1]
						>> vertexIndex[2] >> junk >> uvIndex[2] >> junk >> normalIndex[2];

					vertexIndices.push_back(vertexIndex[0]);
					vertexIndices.push_back(vertexIndex[1]);
					vertexIndices.push_back(vertexIndex[2]);
					uvIndices.push_back(uvIndex[0]);
					uvIndices.push_back(uvIndex[1]);
					uvIndices.push_back(uvIndex[2]);
					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[1]);
					normalIndices.push_back(normalIndex[2]);
				}
				else if (line.substr(0, 6) == "mtllib") { //Save the .mtl filename
					ss.clear();
					ss.str(line);
					ss >> junks >> materialLib;
				}
			}
		}
		file.close();

		if (vertexIndices.size() == 0 || tempVertices.size() == 0) { //Makr sure the obj wasn't empty
			return false;
		}

		this->vertexCount = vertexIndices.size();
		this->indexCount = vertexIndices.size();
		outputIndices = new unsigned long[this->indexCount];
		this->vertPositions = tempVertices;

		for (int i = 0; i < this->vertexCount; i++) { //Create the output vertex array
			Vertex tempVertex;
			tempVertex.position = tempVertices.at(vertexIndices.at(i) - 1);
			tempVertex.texture = tempUvs.at(uvIndices.at(i) - 1);
			tempVertex.normal = tempNormals.at(normalIndices.at(i) - 1);
			outputVertices->push_back(tempVertex);

			outputIndices[i] = i;
		}

		path = "..\\SmallGameProject\\Resources\\OBJ\\";
		path.append(filename);
		path.append(".ace"); //Make the "All Computations Executed" file
		file.open(path, std::ios::out);
		if (!file.is_open()) {
			return false;
		}
		file << this->vertexCount << " " << this->indexCount << "\n"; //Save the sizes, needed for loading the binary files
		file << materialLib << "\n";

		for (int i = 0; i < subsetIndices.size(); i++) {
			file << subsetIndices.at(i) << " " << materialNames.at(i) << "\n"; //Save all subsets with each material
		}
		file.close();

		//Save the vericies as binary
		path = "..\\SmallGameProject\\Resources\\OBJ\\";
		path.append(filename);
		path.append("V.bin");
		file.open(path, std::ios::out | std::ios::binary);
		if (!file.is_open()) {
			return false;
		}
		file.write((char*)&((*outputVertices)[0]), sizeof(Vertex) * outputVertices->size());
		file.close();

		//save the indices as binary
		path = "..\\SmallGameProject\\Resources\\OBJ\\";
		path.append(filename);
		path.append("I.bin");
		file.open(path, std::ios::out | std::ios::binary);
		if (!file.is_open()) {
			return false;
		}
		file.write((char*)outputIndices, sizeof(unsigned long) * this->indexCount);
		file.close();
	}

	return true;
}

const std::vector<DirectX::XMFLOAT3>* Model::getVertexPositions()
{
	return &this->vertPositions;
}