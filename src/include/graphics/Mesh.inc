namespace GL
{
	template<typename Arr>
	void Mesh::SetVertexAttribute(
		const Arr &data,
		uint dataSize,
		uint index, 
		uint drawType,
		uint dataType,
		bool normalized)
	{
		using T = typename Arr::value_type;
	
		if(!VAO)
		{
			glGenVertexArrays(1, &VAO);
		}
	
		uint curVBO = 0;
		auto &it = VBOs.find(index);
		if(it == VBOs.end())
		{
			glGenBuffers(1, &curVBO);
			VBOs.emplace(index, curVBO);
		}
		else
		{
			curVBO = it->second;
		}
	
		glBindVertexArray(VAO);
		{
			glBindBuffer(GL_ARRAY_BUFFER, curVBO);
			glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), drawType);
	
			glVertexAttribPointer(index, dataSize, dataType, normalized, sizeof(T), (void *)0);
			glEnableVertexAttribArray(index);
		}
		glBindVertexArray(0);
	}
}