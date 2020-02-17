#include "Entity.h"
#include "Errorlogger.h"
#include "Input.h"

void Entity::updateMatrix() {
	m_matrixChanged = false;
	float4x4 matTranform = float4x4::CreateTranslation(m_position);
	float4x4 matRotation = float4x4::CreateRotationX(m_rotation.x) *
						   float4x4::CreateRotationY(m_rotation.y) *
						   float4x4::CreateRotationZ(m_rotation.z);
	float4x4 matScale = float4x4::CreateScale(m_scale);
	float4x4 matWorld = matScale * matRotation * matTranform;
	m_matrixBufferData.matWorld = matWorld;
	m_matrixBufferData.matInvTraWorld = matWorld.Invert().Transpose();

	m_collisionData.rotateObbAxis(matRotation);
	m_collisionData.setCollisionPosition(m_position);
}

void Entity::bindModelMatrixBuffer() {
	if (m_matrixChanged) {
		// update matrix if needed
		updateMatrix();
	}
	// fill data to pipeline, transpose matrices
	Entity::MatrixBuffer transposedData = m_matrixBufferData;
	transposedData.matWorld = transposedData.matWorld.Transpose();
	transposedData.matInvTraWorld = transposedData.matInvTraWorld.Transpose();
	Renderer::getDeviceContext()->UpdateSubresource(
		m_modelMatrixBuffer.Get(), 0, 0, &transposedData, 0, 0);
	// bind buffer to pipeline
	Renderer::getDeviceContext()->VSSetConstantBuffers(
		MODEL_MATRIX_BUFFER_SLOT, 1, m_modelMatrixBuffer.GetAddressOf());
}

void Entity::createBuffers() {
	// matrix buffer
	if (m_modelMatrixBuffer.Get() == nullptr) {
		D3D11_BUFFER_DESC desc;
		memset(&desc, 0, sizeof(desc));
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(MatrixBuffer);

		HRESULT res =
			Renderer::getDevice()->CreateBuffer(&desc, nullptr, m_modelMatrixBuffer.GetAddressOf());
		if (FAILED(res))
			ErrorLogger::logError(res, "Entity failed creating matrix buffer!");
	}
}

bool Entity::isMeshInitialized() const { return (m_mesh.get() != nullptr); }

bool Entity::onGround(float height) const { return m_position.y - height < 0.0001; }

float4x4 Entity::getModelMatrix() {
	if (m_matrixChanged)
		updateMatrix();
	return m_matrixBufferData.matWorld;
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
	m_scale = float3(abs(scale.x), abs(scale.y), abs(scale.z));
	m_matrixChanged = true;
}

void Entity::setScale(float scale) {
	m_scale = float3(abs(scale), abs(scale), abs(scale));
	m_matrixChanged = true;
}

void Entity::draw() {
	if (isMeshInitialized()) {
		bindModelMatrixBuffer();
		m_mesh.get()->draw();
	}
}

void Entity::draw_onlyMesh(float3 color) {
	if (isMeshInitialized()) {
		bindModelMatrixBuffer();
		m_mesh.get()->draw_noMaterial(color);
	}
}

void Entity::draw_boundingBox() {
	if (isMeshInitialized()) {
		bindModelMatrixBuffer();
		m_mesh.get()->draw_BoundingBox();
	}
}

void Entity::draw_animate() {
	bindModelMatrixBuffer();
	m_meshAnim.draw();
}

void Entity::updateAnimated(float dt) { m_meshAnim.update(dt); }

void Entity::updateAnimatedSpecific(float frameTime) { m_meshAnim.updateSpecific(frameTime); }

void Entity::setFrameTargets(int first, int second) { m_meshAnim.setFrameTargets(first, second); }

bool Entity::load(string filename) { 
	shared_ptr<Mesh> m = MeshRepository::get(filename);
	if (m.get() != nullptr) {
			m_mesh = m;
		return true;
	}
	else {
		return false;
	}
}

bool Entity::loadAnimated(string filename, int nrOfFrames) {
	return m_meshAnim.load(filename, nrOfFrames);
}

bool Entity::checkCollision(Entity& other) {
	return m_collisionData.collide(other.m_collisionData);
}

float Entity::castRay(float3 rayPos, float3 rayDir) { 
	if (m_mesh.get() != nullptr) {
		float4x4 mWorld = getModelMatrix();
		float4x4 mInvWorld = mWorld.Invert();
		float3 lrayPos = XMVector4Transform(float4(rayPos.x, rayPos.y, rayPos.z, 1), mInvWorld);
		float3 lrayDir = XMVector4Transform(
			float4(rayDir.x, rayDir.y, rayDir.z, 0), mInvWorld.Transpose().Invert());
		lrayDir.Normalize();

		float t = m_mesh->castRayOnMesh(lrayPos, lrayDir);
		if (t > 0) {
			float3 target = XMVector3Transform(lrayPos + lrayDir * t, mWorld);
			return (target.x - rayPos.x) / rayDir.x;
		}
		else
			return -1;
	}
	return -1;
}

void Entity::setCollisionData(float3 point, float radius) {
	m_collisionData.setCollisionData(point, radius);
}

void Entity::setCollisionData(float3 point, float3 halfSizes) {
	m_collisionData.setCollisionData(point, halfSizes);
}

float3 Entity::getHalfSizes() const { return m_mesh.getBoundingBoxHalfSizes(); }

float3 Entity::getHalfSizesAnimated() const { return m_meshAnim.getBoundingBoxHalfSizes(); }

Entity::Entity(string filename, float3 position, float3 rotation, float3 scale) {
	load(filename);
	m_position = position;
	m_rotation = rotation;
	m_scale = scale;
	createBuffers();
}

Entity::~Entity() {}
