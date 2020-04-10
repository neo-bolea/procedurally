#include "Graphics/GLTypes.h"

#include "Common/Debug.h"
#include "Graphics/GL.h"

//TODO: Change draw type!
namespace GL
{
	void Mesh::Setup()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(5, &VBO[0]);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		{
			glBindBuffer(GL::ArrBuffer, VBO[0]);
			glBufferData(GL::ArrBuffer, Pos.size() * sizeof(Vector3), Pos.data(), GL::StaticDraw);

			glVertexAttribPointer(0, 3, GL::Float, false, sizeof(Vector3), 0);
			glEnableVertexAttribArray(0);
		}

		if(!Norms.empty())
		{
			glBindBuffer(GL::ArrBuffer, VBO[1]);
			glBufferData(GL::ArrBuffer, Norms.size() * sizeof(Vector3), Norms.data(), GL::StaticDraw);

			glVertexAttribPointer(1, 3, GL::Float, false, sizeof(Vector3), 0);
			glEnableVertexAttribArray(1);
		}

		if(!UVs.empty())
		{
			glBindBuffer(GL::ArrBuffer, VBO[2]);
			glBufferData(GL::ArrBuffer, UVs.size() * sizeof(Vector2), UVs.data(), GL::StaticDraw);

			glVertexAttribPointer(2, 2, GL::Float, false, sizeof(Vector2), 0);
			glEnableVertexAttribArray(2);
		}

		if(!Tans.empty())
		{
			glBindBuffer(GL::ArrBuffer, VBO[3]);
			glBufferData(GL::ArrBuffer, Tans.size() * sizeof(Vector3), Tans.data(), GL::StaticDraw);

			glVertexAttribPointer(3, 3, GL::Float, false, sizeof(Vector3), 0);
			glEnableVertexAttribArray(3);
		}

		if(!Bitans.empty())
		{
			glBindBuffer(GL::ArrBuffer, VBO[4]);
			glBufferData(GL::ArrBuffer, Bitans.size() * sizeof(Vector3), Bitans.data(), GL::StaticDraw);

			glVertexAttribPointer(4, 3, GL::Float, false, sizeof(Vector3), 0);
			glEnableVertexAttribArray(4);
		}

		if(!Indices.empty())
		{
			glBindBuffer(GL::ElemArrBuffer, EBO);
			glBufferData(GL::ElemArrBuffer, Indices.size() * sizeof(uint), Indices.data(), GL::StaticDraw); 
		}

		glBindBuffer(GL::ArrBuffer, 0);
	}

	void Mesh::Draw()
	{
		int number;
		int typeCounts[TexUse::MAX] = {};
		GL::TexUse type;
		for(uint i = 0; i < Texs.size(); i++)
		{
			glActiveTexture(GL::Texture0 + i);

			type = Texs[i].Type_;
			number = typeCounts[type];
			std::string propString = "uMaterial." + TexTypeStrs[type] + std::to_string(number);

			typeCounts[type]++;

			Program::SetGlobal(propString.c_str(), i);
			glBindTexture(GL_TEXTURE_2D, Texs[i].Ref.ID);
		}

		glBindVertexArray(VAO);
		if(Indices.empty())
		{ glDrawArrays(GL::Triangles, 0, (GLsizei)Pos.size()); }
		else
		{ glDrawElements(GL::Triangles, (GLsizei)Indices.size(), GL::UInt, 0); }
		glBindVertexArray(0);
	}
}