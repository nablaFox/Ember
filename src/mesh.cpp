#include "mesh.hpp"

using namespace etna;

_Mesh::_Mesh(const CreateInfo2& info)
	: m_device(nullptr),
	  vertexBuffer(info.vertexBuffer),
	  indexBuffer(info.indexBuffer) {
	assert(vertexBuffer != IGNIS_INVALID_BUFFER_ID);
	assert(indexBuffer != IGNIS_INVALID_BUFFER_ID);
}

_Mesh::~_Mesh() {
	if (!m_device)
		return;

	m_device->destroyBuffer(vertexBuffer);
	m_device->destroyBuffer(indexBuffer);
}
