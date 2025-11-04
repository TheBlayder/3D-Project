
# PRIMITIVE TOPOLOGY: TRIANGLE LIST

# TODO:
- Deferred Rendering
	+ Multiple Render Targets
	+ G-Buffer (Created but not integrated)
	+ Compute Shader for Lighting (Created but not integrated)

	+ Geometry Pass
	+ Lighting Pass
- Shadow Mapping
- GPU Billboarding Particle system
- Tessellation
- Input System
- Mesh Handling
	+ When rendering a mesh, the vertex buffer can be set once, but each submesh can be renderered seperately using DrawIndexed or DrawIndexedInstanced.
- Dynamic Cube Environment Mapping
	+ The shape of the reflective object must not be a cube/cuboid, e.g. it could be a sphere.
	+ The cube map textures must be stored in a TextureCube resource.
	+ The cube map must be updated each frame.
	+ Some object(s) in the scene must move and be visible on the reflection, to visualize the real-time updating.
	+ Objects rendered in the reflections must be shaded as any other object in the scene, complete with light calculations.
	+ Particles and shadows are not required to be rendered in the reflections.
	+ Culling is not required when generating reflections.