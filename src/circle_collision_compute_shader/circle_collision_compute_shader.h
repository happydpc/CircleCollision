#pragma once

#include "Renderer/vulkan_initializers.hpp"
#include "Renderer/common.hpp"
#include <chrono>

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#define NOMINMAX
#endif

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <optional>

namespace helper
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphics_family;
		std::optional<uint32_t> present_family;
		std::optional<uint32_t> compute_family;

		bool is_complete()
		{
			return graphics_family.has_value() && present_family.has_value() && compute_family.has_value();
		}
	};

	QueueFamilyIndices find_queue_family_indices(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface);

	uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties, VkPhysicalDevice physical_device);

	bool create_buffer(
		VkDevice device,
		VkPhysicalDevice physical_device,
		VkDeviceSize buffer_size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags memory_properties,
		VkBuffer& buffer,
		VkDeviceMemory& buffer_memory);

	bool copy_buffer(
		VkDevice device,
		VkCommandPool stage_command_pool,
		VkQueue queue,
		VkBuffer& src_buffer,
		VkBuffer& dst_buffer,
		VkDeviceSize buffer_size);

	VkShaderModule create_shader_module(VkDevice device, const std::vector<char>& code);
};

struct vertex
{
	glm::vec2 pos;
	glm::vec3 color;
};

struct model
{
	std::vector<vertex> vertices;
	std::vector<uint16_t> indices;
};

struct buffer
{
	VkBuffer buffer;
	VkDeviceMemory device_memory;
	VkDeviceSize offset;
	VkDeviceSize size;

	VkDescriptorBufferInfo get_descriptor_info()
	{
		VkDescriptorBufferInfo info = {};
		info.buffer = this->buffer;
		info.offset = offset;
		info.range = size;
		return info;
	}

	void destroy(const VkDevice& device)
	{
		vkDestroyBuffer(device, this->buffer, nullptr);
		vkFreeMemory(device, this->device_memory, nullptr);
	}
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> present_modes;
};

struct UniformBufferObject
{
	glm::mat4 view;
	glm::mat4 proj;
};

struct circles_strcut
{
	std::vector<glm::vec2> positions;
	std::vector<glm::vec2> velocities;
	std::vector<glm::vec3> colors;
	std::vector<float> scales; // = radius

	inline void resize(const size_t& size)
	{
		positions.resize(size);
		velocities.resize(size);
		colors.resize(size);
		scales.resize(size);
	}
};

struct CircleCollisionComputeShader
{
public:
	void initialize();

	bool run();

	bool release();

	void window_resize();

private:

	bool setup_window();
	bool setup_vulkan();

	bool create_instance();
	bool set_up_debug_messenger();
	bool pick_physical_device();

	// Graphics
	bool check_device_extensions_support();
	bool create_logical_device();
	bool create_surface();
	bool create_swap_chain();
	bool create_image_views();
	bool create_renderpass();
	bool create_descriptor_set_layout();
	bool create_graphics_pipeline();
	bool create_vertex_buffer(); //
	bool create_index_buffer(); //
	bool create_instance_buffers(); //
	bool create_mvp_uniform_buffers(); //
	bool create_descriptor_pool();
	bool create_descriptor_sets();
	bool create_frame_buffers();
	bool create_command_pool();
	bool create_command_buffers();
	bool create_sync_objects();

	bool create_colors_buffer(); //

	bool cleanup_swap_chain();
	bool recreate_swap_chain();
	bool set_viewport_scissor();

	void update(const uint32_t& current_image);

	bool draw_frame();

	bool main_loop();

	// Compute
	bool prepare_compute();
	bool prepare_compute_buffers();
	bool create_compute_positions_buffer();
	bool create_compute_scales_buffer();
	bool create_compute_velocities_buffer();
	bool create_compute_ubo_buffer();
	bool create_compute_command_buffers();

	// TODO: Consider more than 1 swapchain image
	struct
	{
		VkDescriptorSetLayout descriptor_set_layout;
		VkDescriptorSet descriptor_set;

		VkCommandPool command_pool;
		VkCommandBuffer command_buffer;
		VkQueue queue;
		VkFence fence;
		VkPipelineLayout pipeline_layout;
		VkPipeline pipeline;

		buffer ubo_buffer;
		buffer position_buffer;
		buffer scales_buffer;
		buffer velocities_buffer;

		struct
		{
			alignas(alignof(float))		float dt;
			alignas(alignof(int))		int count;
		} ubo;

		struct 
		{
			int right;
			int bottom;
		} push_constant;
	} compute;

	// Sample ----------------

	model circle_model;

	void setup_circles();
	circles_strcut circles;

	VkBuffer vertex_buffer;
	VkDeviceMemory vertex_buffer_memory;

	VkBuffer index_buffer;
	VkDeviceMemory index_buffer_memory;

	VkBuffer colors_buffer;
	VkDeviceMemory colors_buffer_memory;

	std::vector<VkBuffer> ubo_buffers;
	std::vector<VkDeviceMemory> ubo_buffers_memory;

	VkDescriptorPool descriptor_pool;
	std::vector<VkDescriptorSet> ubo_descriptor_sets;
	VkDescriptorSetLayout ubo_descriptor_set_layout;

	VkPipelineLayout pipeline_layout;
	VkPipeline graphics_pipeline;

	VkCommandPool command_pool;
	std::vector<VkCommandBuffer> command_buffers;

	// Sample ----------------


	//	Vulkan
	VkInstance instance = VK_NULL_HANDLE;
	VkPhysicalDevice physical_device = VK_NULL_HANDLE;
	VkDevice  device = VK_NULL_HANDLE;
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	helper::QueueFamilyIndices family_indices;

	VkRenderPass render_pass;

	VkQueue graphics_queue;
	VkQueue present_queue;

	bool should_recreate_swapchain = false;

	VkSwapchainKHR swap_chain;
	std::vector<VkImage> swap_chain_images;
	std::vector<VkImageView> swap_chain_image_views;
	std::vector<VkFramebuffer> swap_chain_frame_buffers;
	VkFormat swap_chain_image_format;
	VkExtent2D swap_chain_extent;

	VkViewport viewport;
	VkRect2D scissor;

	std::vector<VkSemaphore> image_available_semaphore;
	std::vector<VkSemaphore> render_finished_semaphore;
	std::vector<VkFence> draw_fences;

	std::chrono::time_point<std::chrono::high_resolution_clock> last_timestamp;
	size_t frame_counter;
	float frame_timer = 1.0f;
	uint32_t last_fps = 0;

	size_t num_frames;
	size_t current_frame = 0;

	bool validation_layers_enabled;

#ifdef _DEBUG
	VkDebugUtilsMessengerEXT debug_messenger;
#endif

	std::string app_path;

	//	Window
	GLFWwindow* window;

	bool is_released;
};