#pragma once

#include <stdint.h>
#include <math.h>
#include <list>

#ifdef D9VR_EXPORTS
#define D9VR_EXPORT
#else
#define D9VR_EXPORT
#endif

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

	struct Vector2
	{
		float Data[2];
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
			OpenVR,
			Count,
			Invalid = Count
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
			GenericTracker,
			TrackingReference,
			DisplayRedirect,
			Count,
			Invalid = Count
		};
	}
	using DeviceClass = DeviceClasses::DeviceClass;

	namespace Hands
	{
		enum Hand : uint8_t
		{
			Left,
			Right,
			Count,
			Invalid = Count
		};
	}
	using Hand = Hands::Hand;

	struct Pose
	{
		Matrix PoseMatrix;
		Vector Velocity;
		Vector AngularVelocity;

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

	using DeviceId = uint32_t;

	class IGenericDevice
	{
	public:
		virtual DeviceClass GetClass() = 0;
		virtual Pose* GetPose() = 0;
		virtual DeviceId GetDeviceId() = 0;
	};

	using ControllerAxis = Vector2;

	namespace Buttons
	{
		enum Button : uint32_t
		{
			System = 0,
			Menu,
			Grip,
			Left,
			Up,
			Right,
			Down,
			A,
			k_EButton_Axis0 = 32,
			k_EButton_Axis1 = 33,
			k_EButton_Axis2 = 34,
			k_EButton_Axis3 = 35,
			k_EButton_Axis4 = 36,

			Count
		};
	}
	using Button = Buttons::Button;

	class IController : public IGenericDevice
	{
	public:
		virtual bool GetAxis(uint32_t AxisIndex, ControllerAxis* pOutAxis) = 0;
		virtual bool GetButton(Button ButtonIndex) = 0;
		virtual Hand GetHand() = 0;

		const static DeviceClass Class = DeviceClasses::Controller;
	};

	class IHMD : public IGenericDevice
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

		const static DeviceClass Class = DeviceClasses::HMD;
	};

	class ID9VRInterface
	{
	public:
		virtual const std::list<IGenericDevice*>& GetDevices() = 0;

		virtual void SetConversionCallback(MatrixConversionCallback MatrixConvert) = 0;
		virtual void SetLoggingCallback(LoggingCallback Logger) = 0;

		virtual void PollEvents() = 0;

		virtual IGenericDevice* FindDevice(DeviceId Id) = 0;
		virtual IHMD* GetPrimaryHMD() = 0;
		virtual IHMD* GetHMD(uint32_t Index) = 0;
		virtual IController* GetControllerByHand(Hand eHand) = 0;
		virtual IController* GetControllerByIndex(uint32_t Index) = 0;

		virtual bool IsVR() = 0;
	};
}

D9VR_EXPORT d9vr::ID9VRInterface* GetD9VRInterface();
typedef d9vr::ID9VRInterface*(*GetD9VRInterfaceProc)(void);