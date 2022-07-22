#pragma once
#define SPE_KEY_LEN 3 // alt ctrl shift
#define NORM_KEY_LEN 36

#include "support.h"

#include<unordered_map>
#include<functional>


//whole states including | 2 spe_keys | 1 norm_key | mouse |
					//   |	 3 + 1    |   1 + 36   | 1 + 3 |			

struct KeyMouseEvent
{

	int FirstKey;
	int SecondKey;
	int Norm_key = GLFW_KEY_UNKNOWN;
	int mouse;
	bool is_update = false;
	bool is_pressed = false;

	bool operator==(const KeyMouseEvent& p) const {
		return FirstKey == p.FirstKey && SecondKey == p.SecondKey && Norm_key == p.Norm_key && mouse == p.mouse;
	}

	int GenStateDara() const {
		int data = 0;
		data += FirstKey * 1000;
		data += FirstKey * 100;
		data += Norm_key * 1;
		data += mouse * 1;

		return data;
	}
};

struct hash_fn
{
	
	std::size_t operator() (const KeyMouseEvent& inp) const
	{
		std::size_t data = 0;
		data += inp.FirstKey * 1000;
		data += inp.FirstKey * 100;
		data += inp.Norm_key * 1;
		data += inp.mouse * 1;

		return data;
	}
};

class EventListener
{
protected:
	mutable KeyMouseEvent evt_KM;
public:
	bool is_pressed = false;
	mutable double mouse_x, mouse_y, mouse_b_x, mouse_b_y = 0.0f;
	double scr_up, scr_down;

	std::vector<int> evt_IDlist;

	EventListener();
	~EventListener();


	std::unordered_map<KeyMouseEvent, std::function<void()>, hash_fn> EventList;

	int ListenMouseEvent(GLFWwindow* window) const;
	int ListenSpecialKeyEvent(GLFWwindow* window, int ignor) const;
	int ListenNormalKeyEvent(GLFWwindow* window, const std::vector<int>& IDlist) const;
	void UpdateEvent(GLFWwindow* window, const std::vector<int>& IDlist) const;

	KeyMouseEvent GenIntEvent(int k1, int k2, int k3, int m, int upd, int prss);

	//EVENTS
// 	virtual void EventUpdate();
//  
//  	virtual void SHIFT_MMB();
//  	virtual void CTRL_MMB();
//  	virtual void ALT_MMB();
//  	virtual void MMB();
};

