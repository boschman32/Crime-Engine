#pragma once
namespace cr {

	static const int GS_NUM_BUFFERS = 3;

	struct ResourceData
	{
		ResourceData() = default;
		virtual ~ResourceData() {}
	};

	struct TextureData : public ResourceData
	{
		unsigned int m_textureId = 0;
		int m_width = 0;
		int m_height = 0;
	};

	struct ShaderData : public ResourceData
	{

	};

	struct Vertex {

		Vertex(float a_x, float a_y, float a_z) {
			m_pos.x = a_x;
			m_pos.y = a_y;
			m_pos.z = a_z;
		}

		Vertex(glm::vec3 a_vec) {
			m_pos = a_vec;
		}

		glm::vec3 m_pos;
	};

	struct TextVertex
	{
		TextVertex(glm::vec4 a_pos, glm::vec4 a_texCoord, glm::vec4 a_color)
		{
			m_position = a_pos;
			m_texCoord = a_texCoord;
			m_color = a_color;
		}

		glm::vec4 m_position;
		glm::vec4 m_texCoord;
		glm::vec4 m_color;
	};

	struct FontChar
	{
		int m_id;

		float m_u;
		float m_v;

		float m_tWidth;
		float m_tHeight;

		float m_width;
		float m_height;

		float m_xOffset;
		float m_yOffset;
		float m_xAdvance;
	};

	struct FontKerning
	{
		int m_firstId;
		int m_secondId;
		float m_amount;
	};

	struct MeshBuffer {

		std::vector<Vertex> m_vertices;
		std::vector<std::uint16_t> m_indices;
	};

	enum class DrawType {
		Quad = 0,
		Triangle
	};

	struct TextObject
	{
		std::string m_text;

		glm::vec2 m_pos = { 0,0 };
		glm::vec2 m_size = { 1,1 };
		glm::vec2 m_padding = { 0.5f, 0 };
		glm::vec4 m_color = { 1,1,1,1 };
	};

	class Mesh {
	public:
		Mesh(MeshBuffer a_meshBuffer) { m_meshBuffer = a_meshBuffer; }
		virtual ~Mesh() {}
		inline unsigned int GetIndexCount() { return static_cast<unsigned int>(m_meshBuffer.m_indices.size()); }
	protected:

		MeshBuffer m_meshBuffer;

		unsigned int m_vertexByteStride = 0;
		unsigned int m_vertexBufferByteSize = 0;
		unsigned int m_indexBufferByteSize = 0;
	};
}