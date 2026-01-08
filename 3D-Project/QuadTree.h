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


template<typename T>
void QuadTree<T>::PrintTree(const std::unique_ptr<Node>& node)
{
	if (!node)
		return;

	std::cout << "Node Bounds Center: (" << node->bounds.Center.x << ", " << node->bounds.Center.y << ", " << node->bounds.Center.z << ")\n";
	std::cout << "Node Bounds Extents: (" << node->bounds.Extents.x << ", " << node->bounds.Extents.y << ", " << node->bounds.Extents.z << ")\n";
	for (const auto& child : node->children)
	{
		PrintTree(child);
	}
}

template<typename T>
void QuadTree<T>::AddToNode(std::unique_ptr<Node>& node, T* elementAdress)
{
	bool collides = node->bounds.Intersects(elementAdress->GetBoundingBox());
	if (!collides) // If no collision, do not add
		return;

	// Checks if node is a leaf by seeing if it has no children
	bool isLeaf = (!node->children[0]);
	if (isLeaf)
	{
		if (node->elementAdress == nullptr)
			node->elementAdress = elementAdress; // If empty, add element
		else
		{
			SubdivideNode(node); // If occupied, subdivide and re-add existing and new element

			// Redistribute the current element to the appropriate child node
			for (int i = 0; i < 4; ++i)
			{
				AddToNode(node->children[i], node->elementAdress);
			}

			// Clear current element
			node->elementAdress = nullptr;
		}
	}

	// Node is now always a parent, try to add to children
	for (auto& child : node->children)
	{
		AddToNode(child, elementAdress);
	}
}

template<typename T>
void QuadTree<T>::CheckNode(std::unique_ptr<Node>& node, const DirectX::BoundingFrustum& frustum, std::vector<const T*>& foundObjects)
{
	bool collision = frustum.Intersects(node->bounds);
	if (!collision) // If no collision, do not check further
		return;

	// Checks if node is a leaf
	bool isLeaf = (!node->children[0]);
	if (isLeaf)
	{
		if (node->elementAdress)
		{
			//Check if there is a collision between the frustum and the object's bounding volume
			collision = frustum.Intersects(node->elementAdress->GetBoundingBox());
			if (collision)
			{
				// Check if object is already in the list to avoid duplicates
				auto it = std::find(foundObjects.begin(), foundObjects.end(), node->elementAdress);
				if (it == foundObjects.end())
				{
					foundObjects.push_back(node->elementAdress);
				}
			}
		}
	}
	else
	{
		// If not a leaf, check children
		for (auto& child : node->children)
		{
			CheckNode(child, frustum, foundObjects);
		}
	}
}

template<typename T>
void QuadTree<T>::SubdivideNode(std::unique_ptr<Node>& node)
{
	// If occupied, split node and re-add existing and new element
	DirectX::XMVECTOR center = node->bounds.Center;
	DirectX::XMVECTOR extents = node->bounds.Extents;
	DirectX::XMVECTOR halfExtents = { extents.x / 2.0f, extents.y, extents.z / 2.0f };

	// Children bounding volume offsets:
	const DirectX::XMVECTOR offsets[4] =
	{
		{ -halfExtents.x, 0.0f,  halfExtents.z }, // Top-Left
		{  halfExtents.x, 0.0f,  halfExtents.z }, // Bottom-Right
		{ -halfExtents.x, 0.0f, -halfExtents.z }, // Bottom-Left
		{  halfExtents.x, 0.0f, -halfExtents.z }  // Bottom-Right
	};

	// Generate children
	for (int i = 0; i < 4; ++i)
	{
		node->children[i] = std::make_unique<Node>();

		DirectX::XMVECTOR childCenter = DirectX::XMVectorAdd(center, offsets[i]); // Calculate child center

		// Create bounding box for child
		DirectX::BoundingBox::CreateFromPoints(
			node->children[i]->bounds,
			DirectX::XMVectorSubtract(childCenter, halfExtents),
			DirectX::XMVectorAdd(childCenter, halfExtents)
		);
	}

}

template<typename T>
QuadTree<T>::QuadTree()
{
	m_root = std::make_unique<Node>();
	m_root->bounds = DirectX::BoundingBox(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(100.0f, 0.0f, 100.0f));
}

template<typename T>
void QuadTree<T>::AddElement(T* elementAdress)
{
	AddToNode(m_root, elementAdress);
}

template<typename T>
void QuadTree<T>::PrintTree()
{
	PrintTree(m_root);
}

template<typename T>
std::vector<T*> QuadTree<T>::CheckTree(const DirectX::BoundingFrustum& frustum)
{
	std::vector<T*> foundObjects;
	CheckNode(m_root, frustum, foundObjects);

	return foundObjects;
}
