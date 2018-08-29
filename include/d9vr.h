#pragma once

#include <stdint.h>
#include <math.h>

namespace d9vr
{
	struct Matrix
	{
		float Data[4][4];

		inline void Init(
			float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33
		)
		{
			Data[0][0] = m00;
			Data[0][1] = m01;
			Data[0][2] = m02;
			Data[0][3] = m03;

			Data[1][0] = m10;
			Data[1][1] = m11;
			Data[1][2] = m12;
			Data[1][3] = m13;

			Data[2][0] = m20;
			Data[2][1] = m21;
			Data[2][2] = m22;
			Data[2][3] = m23;

			Data[3][0] = m30;
			Data[3][1] = m31;
			Data[3][2] = m32;
			Data[3][3] = m33;
		}
	};

	struct Vector
	{
		float Data[3];
	};

	struct Quaternion
	{
		float Data[4];
	};

	struct Size
	{
		uint32_t Data[2];
	};

	typedef void(*MatrixConversionCallback)(Matrix* pMatrix);
	typedef void(*LoggingCallback)(const char* pMsg);

	namespace Backends
	{
		enum Backend : uint8_t
		{
			OpenVR
		};
	}
	using Backend = Backends::Backend;

	namespace Eyes
	{
		enum Eye : uint8_t
		{
			Left = 0,
			Right = 1,
			Count = 2,
			Invalid = Count
		};
	}
	using Eye = Eyes::Eye;

	namespace DeviceClasses
	{
		enum DeviceClass : uint8_t
		{
			Controller,
			HMD,
			Invalid,
			GenericTracker,
			TrackingReference
		};
	}
	using DeviceClass = DeviceClasses::DeviceClass;

	struct Pose
	{
		Matrix PoseMatrix;
		Vector Velocity;
		Vector AngularVelocity;
		DeviceClass Class;
		//Quaternion Quaternion;

		inline void GetPosition(Vector* pOutPos)
		{
			pOutPos->Data[0] = PoseMatrix.Data[0][3];
			pOutPos->Data[1] = PoseMatrix.Data[1][3];
			pOutPos->Data[2] = PoseMatrix.Data[2][3];
		}

		inline float GetHeight()
		{
			return PoseMatrix.Data[2][3];
		}

		inline void GetQuaternion(Quaternion* pOutQuat)
		{
			pOutQuat->Data[3] = sqrtf(fmaxf(0.0f, 1.0f + PoseMatrix.Data[0][0] + PoseMatrix.Data[1][1] + PoseMatrix.Data[2][2])) / 2.0f;
			pOutQuat->Data[0] = sqrtf(fmaxf(0.0f, 1.0f + PoseMatrix.Data[0][0] - PoseMatrix.Data[1][1] - PoseMatrix.Data[2][2])) / 2.0f;
			pOutQuat->Data[1] = sqrtf(fmaxf(0.0f, 1.0f - PoseMatrix.Data[0][0] + PoseMatrix.Data[1][1] - PoseMatrix.Data[2][2])) / 2.0f;
			pOutQuat->Data[2] = sqrtf(fmaxf(0.0f, 1.0f - PoseMatrix.Data[0][0] - PoseMatrix.Data[1][1] + PoseMatrix.Data[2][2])) / 2.0f;

			pOutQuat->Data[0] = copysignf(pOutQuat->Data[0], PoseMatrix.Data[2][1] - PoseMatrix.Data[1][2]);
			pOutQuat->Data[1] = copysignf(pOutQuat->Data[1], PoseMatrix.Data[0][2] - PoseMatrix.Data[2][0]);
			pOutQuat->Data[2] = copysignf(pOutQuat->Data[2], PoseMatrix.Data[1][0] - PoseMatrix.Data[0][1]);
		}
	};

	class ID9VRHMD
	{
	public:
		virtual float GetIPD() = 0;

		virtual void GetMidEyePose(Matrix* pOutMatrix) = 0;

		virtual void GetEyeRTSize(Size* pOutSize) = 0;
		virtual void GetStereoRTSize(Size* pOutSize) = 0;

		virtual void GetProjectionMatrix(Eye Eye, Matrix* pOutMatrix) = 0;
		virtual void SetZRange(float zNear, float zFar) = 0;

		virtual void GetMidToEyePose(Eye nEye, Matrix* pOutMatrix) = 0;

		virtual void QueueTextureCreationForEye(Eye nEye) = 0;

		virtual void Submit() = 0;
	};

	class ID9VRInterface
	{
	public:
		virtual ID9VRHMD* GetHMD(uint32_t Index) = 0;
		virtual uint32_t GetHMDCount() = 0;

		virtual void SetConversionCallback(MatrixConversionCallback MatrixConvert) = 0;
		virtual void SetLoggingCallback(LoggingCallback Logger) = 0;

		virtual void PollEvents() = 0;

		virtual bool IsVR() = 0;
	};
}

extern "C" d9vr::ID9VRInterface* GetD9VRInterface();
typedef d9vr::ID9VRInterface*(*GetD9VRInterfaceProc)(void);