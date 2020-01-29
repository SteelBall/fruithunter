#include "Entity.h"

void Entity::updateMatrix() {
	m_matrixChanged = false;
	float4x4 matTranform = float4x4::CreateTranslation(m_position);
	float4x4 matRotation = float4x4::CreateRotationZ(m_rotation.z) * float4x4::CreateRotationY(m_rotation.y) *
						   float4x4::CreateRotationX(m_rotation.z);
	float4x4 matScale = float4x4::CreateScale(m_scale);
	m_modelMatrix = matTranform * matRotation * matScale;

	Renderer::getDeviceContext()->UpdateSubresource(m_modelMatrixBuffer, 0, 0, &m_modelMatrix, 0, 0);
}

void Entity::createBuffers() {
	//matrix buffer
	if (m_modelMatrixBuffer == nullptr) {
		D3D11_BUFFER_DESC desc;
		memset(&desc, 0, sizeof(desc));
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(float4x4);

		HRESULT res = Renderer::getDevice()->CreateBuffer(&desc, nullptr, &m_modelMatrixBuffer);
		if (FAILED(res))
			ErrorLogger::logError(res,"Entity failed creating matrix buffer!");
	}
}

void Entity::release() {
	if (m_modelMatrixBuffer != nullptr)
		m_modelMatrixBuffer->Release();
}

float4x4 Entity::getModelMatrix() { 
	if (m_matrixChanged)
		updateMatrix();
	return m_modelMatrix;
}

float3 Entity::getPosition() const { return m_position; }

float3 Entity::getRotation() const { return m_rotation; }

float3 Entity::getScale() const { return m_scale; }

void Entity::setPosition(float3 position) {
	m_position = position;
	m_matrixChanged = true;
}

void Entity::move(float3 movement) {
	m_position += movement;
	m_matrixChanged = true;
}

void Entity::setRotation(float3 rotation) {
	m_rotation = rotation;
	m_matrixChanged = true;
}

void Entity::rotate(float3 rotate) {
	m_rotation += rotate;
	m_matrixChanged = true;
}

void Entity::rotateX(float val) {
	m_rotation.x += val;
	m_matrixChanged = true;
}

void Entity::rotateY(float val) {
	m_rotation.y += val;
	m_matrixChanged = true;
}

void Entity::rotateZ(float val) {
	m_rotation.z += val;
	m_matrixChanged = true;
}

void Entity::setScale(float3 scale) {
	m_scale = scale;
	m_matrixChanged = true;
}

void Entity::setScale(float scale) {
	m_scale = float3(scale, scale, scale);
	m_matrixChanged = true;
}

void Entity::draw() { m_mesh.draw(); }

void Entity::draw_onlyMesh(float3 color) { m_mesh.draw_noMaterial(color); }

bool Entity::load(string filename) {
	if (filename != "") {
		if (!m_mesh.load(filename)) {
			ErrorLogger::logWarning(HRESULT(), "Entity failed at loading mesh: " + filename + "\n");
			return false;
		} else
			return true;
	} else
		return false;
}

Entity::Entity(string filename, float3 position, float3 rotation, float3 scale) {
	load(filename);
	m_position = position;
	m_rotation = rotation;
	m_scale = scale;
	createBuffers();
}

Entity::~Entity() { release(); }
