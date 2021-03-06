#include "IFCJsonInteract.h"
#include "IFCEngineInteract.h"
#include <iostream>
#include <fstream>
#include <string>
#include <locale>
#include <vector>
#include "generic.h"

// need to delete the return char* pointer
char *wchar_t2char(const wchar_t* src)
{
	if (src == NULL)
		return NULL;

	char *str = new char[wcslen(src) + 1];
	wcstombs(str, src, wcslen(src) + 1);
	return str;
}

// need to delete the return wchar_t* pointer
wchar_t *char2wchar_t(const char* src)
{
	if (src == NULL)
		return NULL;

	wchar_t *str = new wchar_t[strlen(src) + 1];
	mbstowcs(str, src, strlen(src) + 1);
	return str;
}

void DumpIfcObjects2SeperateJson(const std::string& fileName)
{
	long int num = 1;

	Json::Value root;
	Json::FastWriter writer;
	STRUCT__IFC__OBJECT	* ifcObject = ifcObjectsLinkedList;
	while (ifcObject) {
		Json::Value JsonIFCObject;
		//GUID
		const char* strGUID = wchar_t2char(ifcObject->globalId);
		JsonIFCObject["globalId"] = strGUID;
		delete strGUID;
		//Type
		const char* strifcType = wchar_t2char(ifcObject->ifcType);
		JsonIFCObject["ifcType"] = strifcType;
		delete strifcType;
		//Instance
		//JsonIFCObject["ifcInstance"] = ifcObject->ifcInstance;
		//Entity
		//JsonIFCObject["ifcEntity"] = ifcObject->ifcEntity;
		//noVertices
		JsonIFCObject["noVertices"] = ifcObject->noVertices;
		//vertices length = 6 * noVertices
		for (int i = 0; i < 6 * ifcObject->noVertices; i++)
		{
			JsonIFCObject["vertices"].append(ifcObject->vertices[i]);
		}
		//noPrimitivesForFaces
		JsonIFCObject["noPrimitivesForFaces"] = ifcObject->noPrimitivesForFaces;
		//indicesForFaces length == 3 * ifcObject->noPrimitivesForFaces
		for (int i = 0; i < 3 * ifcObject->noPrimitivesForFaces; i++)
		{
			JsonIFCObject["indicesForFaces"].append(ifcObject->indicesForFaces[i]);
		}
		//vertexOffsetForFaces
		JsonIFCObject["vertexOffsetForFaces"] = ifcObject->vertexOffsetForFaces;
		/*
		//indexOffsetForFaces
		JsonIFCObject["indexOffsetForFaces"] = ifcObject->indexOffsetForFaces;
		//noPrimitivesForWireFrame
		JsonIFCObject["noPrimitivesForWireFrame"] = ifcObject->noPrimitivesForWireFrame;
		//indicesForLinesWireFrame length == 2 * ifcObject->noPrimitivesForWireFrame
		for (int i = 0; i < 2 * ifcObject->noPrimitivesForWireFrame; i++)
		{
			JsonIFCObject["indicesForLinesWireFrame"].append(ifcObject->indicesForLinesWireFrame[i]);
		}
		//vertexOffsetForWireFrame
		JsonIFCObject["vertexOffsetForWireFrame"] = ifcObject->vertexOffsetForWireFrame;
		//indexOffsetForWireFrame
		JsonIFCObject["indexOffsetForWireFrame"] = ifcObject->indexOffsetForWireFrame;
		*/
		if (ifcObject->noPrimitivesForFaces) {
			std::vector<STRUCT_IFCOBJECT_MATERIAL> ifcObjectMaterialsVector = ifcObject->ifcObjectMaterialsVector;
			for(auto IfcObjectMaterial : ifcObjectMaterialsVector)
			{
				Json::Value JsonIfcObjectMaterial;
				JsonIfcObjectMaterial["indexOffsetForFaces"] = IfcObjectMaterial.indexOffsetForFaces;
				JsonIfcObjectMaterial["indexArrayPrimitives"] = IfcObjectMaterial.indexArrayPrimitives;
				STRUCT_MATERIAL_VALUE materialValue = IfcObjectMaterial.materialValue;
				Json::Value JsonMaterialValue;	
				//ambient
				JsonMaterialValue["ambient_R"] = materialValue.ambient.R;
				JsonMaterialValue["ambient_G"] = materialValue.ambient.G;
				JsonMaterialValue["ambient_B"] = materialValue.ambient.B;
				JsonMaterialValue["ambient_A"] = materialValue.ambient.A;
				//diffuse
				JsonMaterialValue["diffuse_R"] = materialValue.diffuse.R;
				JsonMaterialValue["diffuse_G"] = materialValue.diffuse.G;
				JsonMaterialValue["diffusevB"] = materialValue.diffuse.B;
				JsonMaterialValue["diffuse_A"] = materialValue.diffuse.A;
				//specular
				JsonMaterialValue["specular_R"] = materialValue.specular.R;
				JsonMaterialValue["specular_G"] = materialValue.specular.G;
				JsonMaterialValue["specular_B"] = materialValue.specular.B;
				JsonMaterialValue["specular_A"] = materialValue.specular.A;
				//emissive
				JsonMaterialValue["emissive_R"] = materialValue.emissive.R;
				JsonMaterialValue["emissive_G"] = materialValue.emissive.G;
				JsonMaterialValue["emissive_B"] = materialValue.emissive.B;
				JsonMaterialValue["emissive_A"] = materialValue.emissive.A;
				//other
				JsonMaterialValue["transparency"] = materialValue.transparency;
				JsonMaterialValue["shininess"] = materialValue.shininess;
				//
				JsonIfcObjectMaterial["materialValue"] = JsonMaterialValue;
				JsonIFCObject["ifcObjectMaterials"].append(JsonIfcObjectMaterial);
			}
		}

		root.append(JsonIFCObject);
		if (num%30 == 0)
		{
			const std::string newFileName = fileName + std::to_string(num/30) + std::string(".json");
			std::ofstream ofs;
			ofs.open(newFileName.c_str(), std::ios_base::trunc | std::ios_base::out);
			std::string styledJsonFile = root.toStyledString();
			ofs << styledJsonFile;
			ofs.close();
			root.clear();
		}
		else if (ifcObject->next == NULL)
		{
			const std::string newFileName = fileName + std::to_string(num/30+1) + std::string(".json");
			std::ofstream ofs;
			ofs.open(newFileName.c_str(), std::ios_base::trunc | std::ios_base::out);
			std::string styledJsonFile = root.toStyledString();
			ofs << styledJsonFile;
			ofs.close();
			root.clear();
		}

		ifcObject = ifcObject->next;
		++num;
		//std::cout << "mum == " << num << std::endl;
	}

	/*
	//std::string json_file = writer.write(root);
	std::string styledJsonFile = root.toStyledString();
	std::ofstream ofs;
	ofs.open(fileName.c_str());
	ofs << styledJsonFile;
	*/

	CleanupIfcFile();

	//std::string out = root.toStyledString();
	//std::cout << out << std::endl;
}

void DumpIfcObjects2Json(const std::string& fileName)
{
	//output "["
	std::ofstream ofs;
	ofs.open(fileName.c_str(), std::ios_base::trunc | std::ios_base::out);
	ofs << "\[" << std::endl;
	ofs.close();

	//output ifcObject content
	ofs.open(fileName.c_str(), std::ios_base::app | std::ios_base::out);

	Json::Value root;
	Json::FastWriter writer;
	STRUCT__IFC__OBJECT	* ifcObject = ifcObjectsLinkedList;
	while (ifcObject) {
		Json::Value JsonIFCObject;
		//GUID
		const char* strGUID = wchar_t2char(ifcObject->globalId);
		JsonIFCObject["globalId"] = strGUID;
		delete strGUID;
		//Type
		const char* strifcType = wchar_t2char(ifcObject->ifcType);
		JsonIFCObject["ifcType"] = strifcType;
		delete strifcType;
		//Instance
		//JsonIFCObject["ifcInstance"] = ifcObject->ifcInstance;
		//Entity
		//JsonIFCObject["ifcEntity"] = ifcObject->ifcEntity;
		//noVertices
		JsonIFCObject["noVertices"] = ifcObject->noVertices;
		//vertices length = 6 * noVertices
		for (int i = 0; i < 6 * ifcObject->noVertices; i++)
		{
			JsonIFCObject["vertices"].append(ifcObject->vertices[i]);
		}
		//noPrimitivesForFaces
		JsonIFCObject["noPrimitivesForFaces"] = ifcObject->noPrimitivesForFaces;
		//indicesForFaces length == 3 * ifcObject->noPrimitivesForFaces
		for (int i = 0; i < 3 * ifcObject->noPrimitivesForFaces; i++)
		{
			JsonIFCObject["indicesForFaces"].append(ifcObject->indicesForFaces[i]);
		}

		//root.append(JsonIFCObject);
		std::string styledJsonIFCObject = JsonIFCObject.toStyledString();
		//std::ofstream ofs;
		//ofs.open(fileName.c_str(), std::ios_base::app | std::ios_base::out);
		ofs << styledJsonIFCObject;
		if (ifcObject->next != NULL)
			ofs << "\," << std::endl;

		ifcObject = ifcObject->next;
	}

	ofs << "\]" << std::endl;
	ofs.close();

	/*
	//std::string json_file = writer.write(root);
	std::string styledJsonFile = root.toStyledString();
	std::ofstream ofs;
	ofs.open(fileName.c_str());
	ofs << styledJsonFile;
	*/

	CleanupIfcFile();

	//std::string out = root.toStyledString();
	//std::cout << out << std::endl;
}
