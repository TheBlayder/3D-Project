#include "TestObject.h"

TestObject::TestObject()
{
	m_transform.SetPosition(DX::XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f));
	m_transform.SetRotation(DX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
	m_transform.SetScale(DX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
}
