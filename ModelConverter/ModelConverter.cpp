// ModelConverter.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <Windows.h>
#include <iostream>
#include <fstream>

using namespace std;

struct vertex
{
	float x, y, z;
};

struct face
{
	int vIndex1, vIndex2, vIndex3;
	int uvIndex1, uvIndex2, uvIndex3;
	int nIndex1, nIndex2, nIndex3;
};


void WriteFace(ofstream& fout, const int vertexIndex, const int uvIndex, const int normalIndex,
			   vertex* verticies, vertex* uvs, vertex* normals)
{
	fout << verticies[vertexIndex].x << ' ' << verticies[vertexIndex].y << ' ' << verticies[vertexIndex].z << ' ' <<
		uvs[uvIndex].x << ' ' << uvs[uvIndex].y << ' ' <<
		normals[normalIndex].x << ' ' << normals[normalIndex].y << ' ' << normals[normalIndex].z << endl;
}

int main(int argc, char** va_args)
{
	ifstream fin;
	char fileName[_MAX_PATH];
	char exportedName[_MAX_PATH];
	bool bIsRunning = true;
	while (bIsRunning)
	{
		bool bFileFound = false;
		while (!bFileFound)
		{
			cout << "Enter file to convert." << endl;
			cin >> fileName;
			fin.open(fileName);
			if (fin.good())
			{
				break;
			}
			else
			{
				cout << "File named: " << fileName << " could not be opened." << endl;
				fin.clear();
			}
		}

		
		while (!bFileFound)
		{
			cout << "Enter file name." << endl;
			
			cin >> exportedName;
			if (GetFileAttributesA(exportedName) == INVALID_FILE_ATTRIBUTES)
			{
				break;
			}
			else
			{
				cout << "File named: " << exportedName << " already exists." << endl;
			}
		}

		int vertexCount = 0;
		int uvCount = 0;
		int normalCount = 0;
		int faceCount = 0;
		char input;
		fin.get(input);
		// process file.
		while (!fin.eof())
		{
			if (input == 'v')
			{
				fin.get(input);
				if (input == ' ') ++vertexCount;
				else if (input == 't') ++uvCount;
				else if (input == 'n') ++normalCount;
			}
			else if (input == 'f')
			{
				fin.get(input);
				if (input == ' ') ++faceCount;
			}
			// read rest of line
			while (input != '\n' && !fin.eof())
			{
				fin.get(input);
			}
			// advance to next line.
			fin.get(input);
		}
		// go back to start ?
		fin.clear();
		fin.seekg(ios::beg);

		// construct our file format and output it.
		vertex* verticies = new vertex[vertexCount];
		vertex* uvs = new vertex[uvCount];
		vertex* normals = new vertex[normalCount];
		int vertexIndex = 0;
		int uvIndex = 0;
		int normalIndex = 0;
		int faceIndex = 0;
		face* faces = new face[faceCount];

		if (!verticies || !uvs || !normals || !faces)
		{
			cout << "Error, memory failure" << endl;
			return -1;
		}

		fin.get(input);
		while (!fin.eof())
		{
			if (input == 'v')
			{
				fin.get(input);
				if (input == ' ')
				{
					fin >> verticies[vertexIndex].x >> verticies[vertexIndex].y >> verticies[vertexIndex].z;
					++vertexIndex;
				}
				else if (input == 't')
				{
					fin >> uvs[uvIndex].x >> uvs[uvIndex].y;
					++uvIndex;
				}
				else if (input == 'n')
				{
					fin >> normals[normalIndex].x >> normals[normalIndex].y >> normals[normalIndex].z;
					++normalIndex;
				}
			}
			else if (input == 'f')
			{
				fin.get(input);
				if (input == ' ')
				{
					char garbage;
					fin >> faces[faceIndex].vIndex1 >> garbage >> faces[faceIndex].uvIndex1 >> garbage >> faces[faceIndex].nIndex1;
					fin >> faces[faceIndex].vIndex2 >> garbage >> faces[faceIndex].uvIndex2 >> garbage >> faces[faceIndex].nIndex2;
					fin >> faces[faceIndex].vIndex3 >> garbage >> faces[faceIndex].uvIndex3 >> garbage >> faces[faceIndex].nIndex3;
					++faceIndex;
				}
			}
			while (input != '\n') fin.get(input);

			fin.get(input);
		}

		fin.close();

		ofstream fout;
		fout.open(exportedName);
		fout << "Vertex Count: " << (faceCount * 3) << endl;
		fout << "Index Count: " << (faceCount * 3) << endl;
		fout << endl;
		fout << "Data:" << endl;
		fout << endl;

		for (int i = 0; i < faceIndex; i++)
		{
			WriteFace(fout, (faces[i].vIndex1 - 1), (faces[i].uvIndex1 - 1), (faces[i].nIndex1 - 1),
					  verticies, uvs, normals);
			WriteFace(fout, (faces[i].vIndex2 - 1), (faces[i].uvIndex2 - 1), (faces[i].nIndex2 - 1),
					  verticies, uvs, normals);
			WriteFace(fout, (faces[i].vIndex3 - 1), (faces[i].uvIndex3 - 1), (faces[i].nIndex3 - 1),
					  verticies, uvs, normals);
		}

		delete[] verticies;
		delete[] uvs;
		delete[] normals;
	}
	return 0;
}