#pragma once

#ifdef GAMEAUX_EXPORTS
#define GAMEAUX_API __declspec(dllexport)
#else
#define GAMEAUX_API __declspec(dllimport)
#endif

#include <random>
#include <vector>

constexpr float scr_width{ 1000.0f };
constexpr float scr_height{ 800.0f };
constexpr float sky{ 50.0f };
constexpr float ground{ 800.0f };

constexpr unsigned char zombie1_flag{ 0b00000001 };
constexpr unsigned char zombie2_flag{ 0b00000010 };
constexpr unsigned char zombie3_flag{ 0b00000100 };
constexpr unsigned char hero_flag{ 0b00001000 };

constexpr unsigned char tree1_flag{ 0b00010000 };
constexpr unsigned char tree2_flag{ 0b00100000 };
constexpr unsigned char tree3_flag{ 0b01000000 };

constexpr unsigned char bullet_flag{ 0b10000000 };

enum class dirs { left = 0, right = 1, up = 2, down = 3, stop = 4 };

namespace dll
{
	class GAMEAUX_API RANDOMIZER
	{
		private:
			std::random_device rDevice{};
			std::vector<unsigned int> vSeeds;
			std::seed_seq* sequencer{ nullptr };
			std::mt19937* twister{ nullptr };

		public:

			RANDOMIZER();
			~RANDOMIZER();

			int operator() (int min, int max);
	};

	class GAMEAUX_API PROTON
	{
		protected:
			float width{};
			float height{};
			unsigned char type_flag{ 0 };

		public:
			float x{};
			float y{};
			float ex{};
			float ey{};

			PROTON(float _x = 0, float _y = 0, float _width = 1.0f, float _height = 1.0f);
			virtual ~PROTON() {};

			float GetWidth() const;
			float GetHeight() const;
			void SetWidth(float new_width);
			void SetHeight(float new_height);

			void SetEdges();
			void NewDims(float new_width, float new_height);

			bool GetFlag(unsigned char which_flag) const;
			void SetFlag(unsigned char which_flag);
			void NullFlag(unsigned char which_flag);
			unsigned char GetAllFlags() const;
	};

	class GAMEAUX_API PROT_CONTAINER
	{
		private:
			PROTON* mBase_ptr{ nullptr };
			size_t size{ 0 };
			size_t next_push_position{ 0 };

		public:
			PROT_CONTAINER(size_t lenght);
			~PROT_CONTAINER();

			void push_back(PROTON& element);
			void push_front(PROTON& element);

			PROTON& front()const;
			PROTON& back()const;

			size_t capacity() const;

			PROTON& operator[] (size_t position);
			void operator() (size_t position, PROTON& element);

			float distance(PROTON first, PROTON second) const;
			void distance_sort(PROTON base_point);
	};

	class GAMEAUX_API BASE_CREATURE :public PROTON
	{
		protected:

			float move_x{};
			float move_y{};
			float move_ex{};
			float move_ey{};
			float slope{};
			float intercept{};

			bool hor_line = false;
			bool ver_line = false;

			int current_frame{};
			int max_frames{};
			int frame_delay{};

			int strenght{};
			int attack_delay{};

			float speed{};

			void SetPathInfo(float _end_x, float _end_y);

		public:

			int lifes{ 0 };
			dirs dir{ dirs::right };

			BASE_CREATURE(unsigned char type, float startX, float startY);
			virtual ~BASE_CREATURE() {};
			
			int GetFrame();

			virtual void Release() = 0;
			virtual void Move(bool need_new_path = false, float dest_x = 0, float dest_y = 0, float gear=  1.0f) = 0;
			int Attack();
	};

	typedef BASE_CREATURE* creature_ptr;

	extern GAMEAUX_API creature_ptr CreatureFactory(unsigned char what, float initial_x, float initial_y,
		float bull_end_x = 0, float bull_end_y = 0, int bullet_strenght = 1.0f);
}