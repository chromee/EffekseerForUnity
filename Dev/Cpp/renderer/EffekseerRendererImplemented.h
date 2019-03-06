
#pragma once

#include <EffekseerRenderer.Renderer.h>
#include <EffekseerRenderer.RenderStateBase.h>
#include <EffekseerRenderer.StandardRenderer.h>
#include <EffekseerRenderer.SpriteRendererBase.h>
#include <EffekseerRenderer.RibbonRendererBase.h>
#include <EffekseerRenderer.TrackRendererBase.h>
#include <EffekseerRenderer.RingRendererBase.h>
#include <EffekseerRenderer.ModelRendererBase.h>

#include <map>

#include "../common/IUnityInterface.h"

extern "C"
{
	struct UnityRenderParameter
	{
		//! 0 - procedual, 1 - model
		int RenderMode = 0;

		//! 0 - False, 1 - True 
		int IsDistortingMode = 0;

		//! VertexBuffer 
		int VertexBufferOffset = 0;

		//! Element count (Triangle) or instance
		int32_t ElementCount;

		int ZTest = 0;

		int ZWrite = 0;

		int Blend = 0;

		int Culling = 0;

		//! Texture ptr
		void* TexturePtrs[4];

		int TextureFilterTypes[4];

		int TextureWrapTypes[4];

		//! Material ptr
		void* MaterialPtr = nullptr;

		//! Model ptri
		void* ModelPtr = nullptr;
	};

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API GetUnityRenderParameter(UnityRenderParameter* dst, int index);
	UNITY_INTERFACE_EXPORT int UNITY_INTERFACE_API GetUnityRenderCount();
	UNITY_INTERFACE_EXPORT void* UNITY_INTERFACE_API GetUnityVertexBuffer();
	UNITY_INTERFACE_EXPORT void* UNITY_INTERFACE_API GetUnityInfoBuffer();
	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API SetMaterial(void* material);
	
};

namespace EffekseerRendererUnity
{
	class RendererImplemented;
	class RenderState;
	class VertexBuffer;
	class IndexBuffer;
	class Shader;

	class Model : public Effekseer::Model
	{
	public:
		Model(void* data, int32_t size)
		 : Effekseer::Model(data,size)
		{
		}

		virtual ~Model() = default;

		void* InternalPtr = nullptr;
	};

	struct Vertex
	{
		::Effekseer::Vector3D	Pos;
		::Effekseer::Color	Col;
		float		UV[2];

		void SetColor(const ::Effekseer::Color& color)
		{
			Col = color;
		}
	};

	struct VertexDistortion
	{
		::Effekseer::Vector3D	Pos;
		::Effekseer::Color	Col;
		float		UV[2];
		::Effekseer::Vector3D	Tangent;
		::Effekseer::Vector3D	Binormal;

		void SetColor(const ::Effekseer::Color& color)
		{
			Col = color;
		}
	};

	struct ModelParameter
	{
		Effekseer::Matrix44 Matrix;
		Effekseer::Color VertexColors;
		Effekseer::RectF UV;
		int32_t Time;
	};

	typedef ::Effekseer::ModelRenderer::NodeParameter efkModelNodeParam;
	typedef ::Effekseer::ModelRenderer::InstanceParameter efkModelInstanceParam;
	typedef ::Effekseer::Vector3D efkVector3D;

	class ModelRenderer
		: public ::EffekseerRenderer::ModelRendererBase
	{
	private:
		RendererImplemented*	m_renderer;
		ModelRenderer(RendererImplemented* renderer);

	public:

		virtual ~ModelRenderer();

		static ModelRenderer* Create(RendererImplemented* renderer);

	public:
		void BeginRendering(const efkModelNodeParam& parameter, int32_t count, void* userData) override;

		void Rendering(const efkModelNodeParam& parameter, const efkModelInstanceParam& instanceParameter, void* userData) override;

		void EndRendering(const efkModelNodeParam& parameter, void* userData) override;
	};

	class RendererImplemented
		: public ::EffekseerRenderer::Renderer
		, public ::Effekseer::ReferenceObject
	{
	protected:
		::Effekseer::Vector3D	m_lightDirection;
		::Effekseer::Color		m_lightColor;
		::Effekseer::Color		m_lightAmbient;
		int32_t					m_squareMaxCount;

		::Effekseer::Matrix44	m_proj;
		::Effekseer::Matrix44	m_camera;
		::Effekseer::Matrix44	m_cameraProj;

		::Effekseer::Vector3D	m_cameraPosition;
		::Effekseer::Vector3D	m_cameraFrontDirection;

		VertexBuffer*			m_vertexBuffer = nullptr;
		Shader*					m_stanShader = nullptr;
		Shader*					m_distortionShader = nullptr;
		Shader*					m_currentShader = nullptr;
		RenderState*			m_renderState = nullptr;

		std::array<void*, 16>	m_textures;

		std::vector<UnityRenderParameter> renderParameters;
		std::vector<ModelParameter> modelParameters;

		bool					m_isDistorting = false;
		float					m_distortionIntensity = 0.0f;
		bool					m_isLighting = false;

		std::vector<uint8_t> exportedVertexBuffer;
		std::vector<uint8_t> exportedInfoBuffer;

		Effekseer::TextureData backgroundData;

		EffekseerRenderer::StandardRenderer<RendererImplemented, Shader, Vertex, VertexDistortion>*	m_standardRenderer = nullptr;
	public:
		
		static RendererImplemented* Create();

		RendererImplemented();
		virtual ~RendererImplemented();

		void OnLostDevice() override {}
		void OnResetDevice() override {}

		/**
		@brief	������
		*/
		bool Initialize(int32_t squareMaxCount);

		/**
		@brief	���̃C���X�^���X��j������B
		*/
		void Destroy() override;

		/**
		@brief	�X�e�[�g�𕜋A���邩�ǂ����̃t���O��ݒ肷��B
		*/
		void SetRestorationOfStatesFlag(bool flag) override;

		/**
		@brief	�`����J�n���鎞�Ɏ��s����B
		*/
		bool BeginRendering() override;

		/**
		@brief	�`����I�����鎞�Ɏ��s����B
		*/
		bool EndRendering() override;

		/**
		@brief	���C�g�̕������擾����B
		*/
		const ::Effekseer::Vector3D& GetLightDirection() const override;

		/**
		@brief	���C�g�̕�����ݒ肷��B
		*/
		void SetLightDirection(const ::Effekseer::Vector3D& direction) override;

		/**
		@brief	���C�g�̐F���擾����B
		*/
		const ::Effekseer::Color& GetLightColor() const override;

		/**
		@brief	���C�g�̐F��ݒ肷��B
		*/
		void SetLightColor(const ::Effekseer::Color& color) override;

		/**
		@brief	���C�g�̊����̐F���擾����B
		*/
		const ::Effekseer::Color& GetLightAmbientColor() const override;

		/**
		@brief	���C�g�̊����̐F��ݒ肷��B
		*/
		void SetLightAmbientColor(const ::Effekseer::Color& color) override;

		/**
		@brief	�ő�`��X�v���C�g�����擾����B
		*/
		int32_t GetSquareMaxCount() const  override;

		/**
		@brief	���e�s����擾����B
		*/
		const ::Effekseer::Matrix44& GetProjectionMatrix() const  override;

		/**
		@brief	���e�s���ݒ肷��B
		*/
		void SetProjectionMatrix(const ::Effekseer::Matrix44& mat)  override;

		/**
		@brief	�J�����s����擾����B
		*/
		const ::Effekseer::Matrix44& GetCameraMatrix() const  override;

		/**
		@brief	�J�����s���ݒ肷��B
		*/
		void SetCameraMatrix(const ::Effekseer::Matrix44& mat)  override;

		/**
		@brief	�J�����v���W�F�N�V�����s����擾����B
		*/
		::Effekseer::Matrix44& GetCameraProjectionMatrix()  override;

		::Effekseer::Vector3D GetCameraFrontDirection() const  override;

		/**
		@brief	Get a position of camera
		*/
		::Effekseer::Vector3D GetCameraPosition() const  override;

		/**
		@brief	Set a front direction and position of camera manually
		@note
		These are set based on camera matrix automatically.
		It is failed on some platform.
		*/
		void SetCameraParameter(const ::Effekseer::Vector3D& front, const ::Effekseer::Vector3D& position)  override;

		/**
		@brief	�X�v���C�g�����_���[�𐶐�����B
		*/
		::Effekseer::SpriteRenderer* CreateSpriteRenderer() override;

		/**
		@brief	���{�������_���[�𐶐�����B
		*/
		::Effekseer::RibbonRenderer* CreateRibbonRenderer() override;

		/**
		@brief	�����O�����_���[�𐶐�����B
		*/
		::Effekseer::RingRenderer* CreateRingRenderer() override;

		/**
		@brief	���f�������_���[�𐶐�����B
		*/
		::Effekseer::ModelRenderer* CreateModelRenderer() override;

		/**
		@brief	�O�Ճ����_���[�𐶐�����B
		*/
		::Effekseer::TrackRenderer* CreateTrackRenderer() override;

		/**
		@brief	�W���̃e�N�X�`���Ǎ��N���X�𐶐�����B
		*/
		::Effekseer::TextureLoader* CreateTextureLoader(::Effekseer::FileInterface* fileInterface = NULL)  override;

		/**
		@brief	�W���̃��f���Ǎ��N���X�𐶐�����B
		*/
		::Effekseer::ModelLoader* CreateModelLoader(::Effekseer::FileInterface* fileInterface = NULL) override;

		/**
		@brief	�����_�[�X�e�[�g�������I�Ƀ��Z�b�g����B
		*/
		void ResetRenderState() override;

		/**
		@brief	�w�i��c�܂���G�t�F�N�g���`�悳���O�ɌĂ΂��R�[���o�b�N���擾����B
		*/
		::EffekseerRenderer::DistortingCallback* GetDistortingCallback()  override;

		/**
		@brief	�w�i��c�܂���G�t�F�N�g���`�悳���O�ɌĂ΂��R�[���o�b�N��ݒ肷��B
		*/
		void SetDistortingCallback(::EffekseerRenderer::DistortingCallback* callback) override;

		int32_t GetDrawCallCount() const override { return 0; }

		int32_t GetDrawVertexCount() const override { return 0; }

		void ResetDrawCallCount() override { }

		void ResetDrawVertexCount() override { }

		void SetRenderMode(Effekseer::RenderMode renderMode) override { }

		Effekseer::RenderMode GetRenderMode() override { return Effekseer::RenderMode::Normal; }

		Effekseer::TextureData* GetBackground();

		void SetBackground(void* image);

		VertexBuffer* GetVertexBuffer();

		IndexBuffer* GetIndexBuffer();

		EffekseerRenderer::StandardRenderer<RendererImplemented, Shader, Vertex, VertexDistortion>* GetStandardRenderer();

		::EffekseerRenderer::RenderStateBase* GetRenderState();

		void SetVertexBuffer(VertexBuffer* vertexBuffer, int32_t size);
		void SetIndexBuffer(IndexBuffer* indexBuffer);

		void SetLayout(Shader* shader);
		void DrawSprites(int32_t spriteCount, int32_t vertexOffset);
		
		void DrawModel(void* model, std::vector<Effekseer::Matrix44>& matrixes, std::vector<Effekseer::RectF>& uvs, std::vector<Effekseer::Color>& colors, std::vector<int32_t>& times);

		Shader* GetShader(bool useTexture, bool useDistortion) const;

		void BeginShader(Shader* shader);
		void EndShader(Shader* shader);

		void SetVertexBufferToShader(const void* data, int32_t size);

		void SetPixelBufferToShader(const void* data, int32_t size);

		void SetTextures(Shader* shader, Effekseer::TextureData** textures, int32_t count);
		void SetIsLighting(bool value) { m_isLighting = value; }
		void SetIsDistorting(bool value) { m_isDistorting = value; }
		void SetDistortionIntensity(float value) { m_distortionIntensity = value; }

		void* FindMaterial();
		void SetMaterial(void* material);

		std::vector<UnityRenderParameter>& GetRenderParameters() { return renderParameters; };
		std::vector<uint8_t>& GetRenderVertexBuffer() { return exportedVertexBuffer; }
		std::vector<uint8_t>& GetRenderInfoBuffer() { return exportedInfoBuffer; }

		virtual int GetRef() { return ::Effekseer::ReferenceObject::GetRef(); }
		virtual int AddRef() { return ::Effekseer::ReferenceObject::AddRef(); }
		virtual int Release() { return ::Effekseer::ReferenceObject::Release(); }
	};

}