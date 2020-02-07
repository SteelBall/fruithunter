#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "SimpleMath.h"
#include "ErrorLogger.h"
#include "Renderer.h"
#define MATRIX_SLOT 1

class Camera {
public:
	Camera();
	Camera(Vector3 camEye, Vector3 camTarget, Vector3 camUp);
	~Camera();

	void setEye(DirectX::SimpleMath::Vector3 camEye);
	void setTarget(DirectX::SimpleMath::Vector3 camTarget);
	void setUp(DirectX::SimpleMath::Vector3 camUp);
	void setView(DirectX::SimpleMath::Vector3 camEye, DirectX::SimpleMath::Vector3 camTarget,
		DirectX::SimpleMath::Vector3 camUp);

	void updateBuffer();
	void bindMatrix();
	DirectX::SimpleMath::Matrix getViewProjMatrix() const;

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_matrixBuffer;
	ErrorLogger m_errors;

	bool m_viewChanged = false;
	DirectX::SimpleMath::Matrix m_vpMatrix;
	DirectX::SimpleMath::Matrix m_viewMatrix;
	DirectX::SimpleMath::Matrix m_projMatrix;

	DirectX::SimpleMath::Vector3 m_camEye;
	DirectX::SimpleMath::Vector3 m_camTarget;
	DirectX::SimpleMath::Vector3 m_camUp;
};