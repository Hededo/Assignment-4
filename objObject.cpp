#pragma once

//libraries go here:
#include <sb7.h>
#include <vmath.h>
#include <string>
#include <vector>
#include <cctype>

std::vector<int> IntsInString(std::string str)
{
	std::vector<int> toReturn;
	std::string intAsString = "";

	for (char& c : str)
	{
		if (std::isdigit(c))
		{
			intAsString += c;
		}
		else
		{
			if (intAsString != "")
			{
				toReturn.push_back(std::stoi(intAsString));
			}
			intAsString = "";
		}
	}

	if (intAsString != "")
	{
		toReturn.push_back(std::stoi(intAsString));
	}

	return toReturn;
}

class ObjObject
{

public:
	std::string fileName;
	std::vector<GLfloat> * verticies;
	std::vector<GLfloat> * normals;
	GLuint vertexCount;

	GLuint vao;
	GLuint vertexbuffer;
	GLuint colorBuffer;
	GLuint normalsBuffer;

	ObjObject::ObjObject(std::string _fileName)
	{
		fileName = _fileName;
		verticies = new std::vector<GLfloat>;
		normals = new std::vector<GLfloat>;
		vertexCount = 0;
		CreateFromFile();
	}

	ObjObject::~ObjObject()
	{
		delete(verticies);
		delete(normals);
		verticies = NULL;
		normals = NULL;
	}

	void ObjObject::BindBuffers()
	{
		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glEnableVertexAttribArray(0); //enable or disable a generic vertex attribute array
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0); //define an array of generic vertex attribute data void glVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)

		glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
		glEnableVertexAttribArray(1); //enable or disable a generic vertex attribute array
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0); //define an array of generic vertex attribute data void glVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)

		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glEnableVertexAttribArray(2); //enable or disable a generic vertex attribute array
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0); //define an array of generic vertex attribute data void glVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
	}

	void ObjObject::Draw()
	{
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	}

private:
	void ObjObject::CreateFromFile()
	{
		std::ifstream file = std::ifstream(fileName);
		std::string line;

		std::vector<vmath::vec3> * objVecticies = new std::vector<vmath::vec3>;
		std::vector<vmath::vec3> * objNormals = new std::vector<vmath::vec3>;

		std::vector<GLfloat> * faceVerticiesIndex = new std::vector<GLfloat>;
		std::vector<GLfloat> * faceNormalIndex = new std::vector<GLfloat>;

		while (std::getline(file, line))
		{
			std::stringstream stream(line);
			std::string firstWord;
			stream >> firstWord;
			std::string a, b, c;
			std::vector<int> vert0, vert1, vert2;
			int vertexIndex, normalIndex;

			bool matches = (std::strcmp(firstWord.c_str(), "v")) == 0;
			if (matches)
			{
				stream >> a >> b >> c;
				objVecticies->push_back(vmath::vec3(std::stof(a), std::stof(b), std::stof(c)));
				continue;
			}
			matches = (std::strcmp(firstWord.c_str(), "vn")) == 0;
			if (matches)
			{
				stream >> a >> b >> c;
				objNormals->push_back(vmath::vec3(std::stof(a), std::stof(b), std::stof(c)));
				continue;
			}
			matches = (std::strcmp(firstWord.c_str(), "f")) == 0;
			if (matches)
			{
				stream >> a >> b >> c;
				vert0 = IntsInString(a);
				vert1 = IntsInString(b);
				vert2 = IntsInString(c);

				//Decrement each index by 1 because OBJ indexs start at 1, not 0 like c++
				faceVerticiesIndex->push_back(vert0.at(0) - 1);
				faceNormalIndex->push_back(vert0.at(1) - 1);
				faceVerticiesIndex->push_back(vert1.at(0) - 1);
				faceNormalIndex->push_back(vert1.at(1) - 1);
				faceVerticiesIndex->push_back(vert2.at(0) - 1);
				faceNormalIndex->push_back(vert2.at(1) - 1);
				continue;
			}
			else
			{
				continue;
			}
		}

		for (int i = 0; i < faceVerticiesIndex->size(); i++)
		{
			int index = faceVerticiesIndex->at(i);
			vmath::vec3 valueAtIndex = objVecticies->at(index);
			verticies->push_back(valueAtIndex[0]);
			verticies->push_back(valueAtIndex[1]);
			verticies->push_back(valueAtIndex[2]);
			verticies->push_back(1.0f);
			vertexCount += 1;
		}

		for (int i = 0; i < faceNormalIndex->size(); i++)
		{
			int index = faceNormalIndex->at(i);
			vmath::vec3 valueAtIndex = objNormals->at(index);
			normals->push_back(valueAtIndex[0]);
			normals->push_back(valueAtIndex[1]);
			normals->push_back(valueAtIndex[2]);
			normals->push_back(1.0f);
		}


		delete(objVecticies);
		delete(objNormals);
		delete(faceVerticiesIndex);
		delete(faceNormalIndex);

		objVecticies = NULL;
		objNormals = NULL;
		faceVerticiesIndex = NULL;
		faceNormalIndex = NULL;

		glGenVertexArrays(1, &vao);  //glGenVertexArrays(n, &array) returns n vertex array object names in arrays
		glBindVertexArray(vao); //glBindVertexArray(array) binds the vertex array object with name array.

		GLuint bufferSize = verticies->size() * sizeof(GLfloat);
#pragma region Cube Pos Buffer
		glGenBuffers(1, &vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER,
			bufferSize,
			verticies->data(),
			GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
#pragma endregion

#pragma region Cube Color Buffer
		glGenBuffers(1, &colorBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glBufferData(GL_ARRAY_BUFFER,
			bufferSize,
			verticies->data(),
			GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
#pragma endregion

#pragma region Cube Normals Buffer
		glGenBuffers(1, &normalsBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
		glBufferData(GL_ARRAY_BUFFER,
			bufferSize,
			normals->data(),
			GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
#pragma endregion
	}
};
