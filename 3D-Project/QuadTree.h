#pragma once

#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <memory>
#include <iostream>

/// <summary>
/// Class assuming that T has a public method "GetBoundingBox()" returning a DirectX::BoundingBox&
/// </summary>
template<typename T>
class QuadTree
{
private:

	struct Node 
	{
		T* elementAdress;
		std::unique_ptr<Node> children[4];
		DirectX::BoundingBox bounds;
	};

	std::unique_ptr<Node> m_root;

	void PrintTree(const std::unique_ptr<Node>& node);
	void AddToNode(std::unique_ptr<Node>& node, T* elementAdress);
	void CheckNode(std::unique_ptr<Node>& node, const DirectX::BoundingFrustum& frustum, std::vector<const T*>& foundObjects);

	void SubdivideNode(std::unique_ptr<Node>& node);

public:
	QuadTree();
	void AddElement(T* elementAdress);
	void PrintTree();
	std::vector<T*> CheckTree(const DirectX::BoundingFrustum& frustum);
};