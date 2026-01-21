#pragma once

#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <memory>
#include <iostream>
#include <queue>

/// <summary>
/// Class assuming that T has a public method "GetBoundingBox()" returning a DirectX::BoundingBox&
/// </summary>
template<typename T>
class QuadTree
{
private:

	struct Node 
	{
		T* elementAdress = nullptr;
		std::unique_ptr<Node> children[4];
		DirectX::BoundingBox bounds;
	};

	std::unique_ptr<Node> m_root;
	size_t m_maxDepth;

	void AddToNode(std::unique_ptr<Node>& node, T* elementAdress, size_t currentDepth);
	void CheckNode(std::unique_ptr<Node>& node, const DirectX::BoundingFrustum& frustum, std::vector<const T*>& foundObjects);

	void SubdivideNode(std::unique_ptr<Node>& node);

	void PrintNode(std::unique_ptr<Node>& node, size_t depth);

public:
	QuadTree(size_t maxDepth);
	QuadTree() : m_maxDepth(0), m_root(nullptr) {}
	void AddElement(T* elementAdress);
	void PrintTree();
	std::vector<T*> CheckTree(const DirectX::BoundingFrustum& frustum);
};

template<typename T>
void QuadTree<T>::AddToNode(std::unique_ptr<Node>& node, T* elementAdress, size_t currentDepth)
{
	if(currentDepth >= m_maxDepth)
	{
		// Max depth reached, add element here
		if (node->elementAdress == nullptr)
		{
			node->elementAdress = elementAdress;
		}
		return;
	}

	bool collides = elementAdress->GetBoundingBox().Intersects(node->bounds);
	if (!collides) // If no collision, do not add
		return;

	// Checks if node is a leaf by seeing if it has no children
	bool isLeaf = (node->children[0] == nullptr);
	if (isLeaf)
	{
		if (node->elementAdress == nullptr) 
		{
			node->elementAdress = elementAdress; // If empty, add element
			return; 
		}
		else
		{
			this->SubdivideNode(node); // If occupied, subdivide and re-add existing and new element

			// Redistribute the current element to the appropriate child node
			for (int i = 0; i < 4; ++i)
			{
				this->AddToNode(node->children[i], node->elementAdress, currentDepth + 1);
			}

			node->elementAdress = nullptr;
		}
	}

	// Node is now always a parent, try to add to children
	for (auto& child : node->children)
	{
		this->AddToNode(child, elementAdress, currentDepth + 1);
	}
}

template<typename T>
void QuadTree<T>::CheckNode(std::unique_ptr<Node>& node, const DirectX::BoundingFrustum& frustum, std::vector<const T*>& foundObjects)
{
	bool collision = frustum.Intersects(node->bounds);
	if (!collision) // If no collision, do not check further
		return;

	// Checks if node is a leaf
	bool isLeaf = (node->children[0] == nullptr);
	if (isLeaf)
	{
		if (node->elementAdress != nullptr)
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
			if(child == nullptr) continue;
			this->CheckNode(child, frustum, foundObjects);
		}
	}
}

template<typename T>
void QuadTree<T>::SubdivideNode(std::unique_ptr<Node>& node)
{
	using namespace DirectX;

	XMVECTOR center = XMLoadFloat3(&node->bounds.Center);
	XMVECTOR extents = XMLoadFloat3(&node->bounds.Extents);
	XMVECTOR halfExtents = XMVectorMultiply(extents, {0.5f, 1.f, 0.5f});

	// Children bounding volume offsets:
	const XMVECTOR offsets[4] =
	{
		XMVectorSet(-XMVectorGetX(halfExtents), 0.0f, -XMVectorGetZ(halfExtents), 0.0f), // Bottom-Left
		XMVectorSet(XMVectorGetX(halfExtents), 0.0f, -XMVectorGetZ(halfExtents), 0.0f),  // Bottom-Right
		XMVectorSet(-XMVectorGetX(halfExtents), 0.0f,  XMVectorGetZ(halfExtents), 0.0f), // Top-Left
		XMVectorSet(XMVectorGetX(halfExtents), 0.0f,  XMVectorGetZ(halfExtents), 0.0f)	// Top-Right
	};

	// Generate children
	for (int i = 0; i < 4; ++i)
	{
		node->children[i] = std::make_unique<Node>();

		XMVECTOR childCenter = XMVectorAdd(center, offsets[i]); // Calculate child center

		// Create bounding box for child
		BoundingBox::CreateFromPoints(
			node->children[i]->bounds,
			XMVectorSubtract(childCenter, halfExtents),
			XMVectorAdd(childCenter, halfExtents)
		);
	}

}

template<typename T>
inline void QuadTree<T>::PrintNode(std::unique_ptr<Node>& node, size_t depth)
{
	if (!node) return;

	std::queue<std::pair<Node*, int>> queue;
	queue.push({ node.get(), depth });

	while (!queue.empty()) {
		auto [currentNode, currentDepth] = queue.front();
		queue.pop();

		// Indentation based on depth
		std::cout << std::string(currentDepth * 2, ' ') << "Node at depth " << currentDepth << ":\n";

		// Bounding box details
		std::cout << std::string(currentDepth * 2, ' ') << "  BoundingBox Center: ("
			<< currentNode->bounds.Center.x << ", " << currentNode->bounds.Center.y << ", "
			<< currentNode->bounds.Center.z << ")\n";
		std::cout << std::string(currentDepth * 2, ' ') << "  BoundingBox Extents: ("
			<< currentNode->bounds.Extents.x << ", " << currentNode->bounds.Extents.y << ", "
			<< currentNode->bounds.Extents.z << ")\n";

		// Print element address if present
		if (currentNode->elementAdress) {
			std::cout << std::string(currentDepth * 2, ' ') << " Element Address: " << currentNode->elementAdress
				<< "\n";
		}
		else {
			std::cout << std::string(currentDepth * 2, ' ') << " Element Address: None\n";
		}

		for (int i = 0; i < 4; ++i) {
			if (currentNode->children[i]) {
				queue.push({ currentNode->children[i].get(), currentDepth + 1 });
			}
		}
	}
}

template<typename T>
QuadTree<T>::QuadTree(size_t maxDepth) : m_maxDepth(maxDepth)
{
	m_root = std::make_unique<Node>();
	m_root->bounds = DirectX::BoundingBox(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(128.0f, 64.f, 128.0f));
}

template<typename T>
void QuadTree<T>::AddElement(T* elementAdress)
{
	this->AddToNode(m_root, elementAdress, 0);
}

template<typename T>
void QuadTree<T>::PrintTree()
{
	this->PrintNode(m_root, 0);
}

template<typename T>
std::vector<T*> QuadTree<T>::CheckTree(const DirectX::BoundingFrustum& frustum)
{
	std::vector<const T*> foundObjects;
	this->CheckNode(m_root, frustum, foundObjects);

	// Convert std::vector<const T*> to std::vector<T*>
	std::vector<T*> result;
	result.reserve(foundObjects.size());
	for (const T* ptr : foundObjects)
	{
		result.push_back(const_cast<T*>(ptr));
	}

	return result;
}
