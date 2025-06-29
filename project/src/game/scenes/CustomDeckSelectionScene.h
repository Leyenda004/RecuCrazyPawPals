#pragma once

#include "Scene.h"
#include <vector>
#include <string>
#include <list>
#include <array>

class ImageForButton;
class Card;
using LastButton = ImageForButton*;
class CustomDeckSelectionScene : public Scene
{
public:
	CustomDeckSelectionScene();
	virtual ~CustomDeckSelectionScene();
	void initScene() override;
	void enterScene() override;
	void exitScene() override;
	void render() override;
	void update(uint32_t delta_time) override;

private:
	void create_weapon_button(GameStructs::WeaponType wt, const GameStructs::ButtonProperties& bp);
	void create_deck_button(GameStructs::DeckType dt,const GameStructs::ButtonProperties& bp);
	void create_back_button(const GameStructs::ButtonProperties& bp);
	void create_weapon_buttons();
	void create_deck_buttons();
	// void create_deck_info(const rect_f32& rect); // Ya no se usa
	// void create_deck_infos(); // Ya no se usa
	void set_concrete_deck_info(const std::list<Card*>& cl);
	void create_weapon_info();
	void create_enter_button();
	inline void weapon_selected() { _weapon_selected = true; };
	inline void deck_selected() { _deck_selected = true; };
	//flags to control if both has been selected
	bool _weapon_selected;
	bool _deck_selected;

	//last pressed button (visual)
	LastButton _last_weapon_button;
	LastButton _last_deck_button;

	// MANAGER
	ecs::Manager* mngr;

	bool _activate_play_button;

	// Variables para el deck
    float umbral = 0.14f;
	rect_f32 or_display_rect = {{ 0.0375f, 0.385f }, { 0.325f, 0.14f }};
	rect_f32 display_rect = or_display_rect; // Modificable para colocar elementos
	int current_array_index = 0;
    static constexpr int CARDS_PER_ARRAY = 4;
    
    // Vector de arrays, cada array tiene exactamente 4 slots (pueden ser nullptr)
    std::vector<std::array<Card*, CARDS_PER_ARRAY>> deck_arrays;

	void reset();

	// Métodos para gestión del mazo
	void assign_default_deck();
    // void delete_default_card();
    void add_card_to_deck(Card *card);
    void remove_last_card_from_deck();

    void refresh_deck_visual(int);
    void organize_deck_into_arrays();
    void update_visual_display();

	// Métodos para mostrar cartas y botón de eliminar
	void create_card_buttons();
	void create_remove_card_button();
	void create_arrow_buttons();
	void change_page(int delta);
	
	// Devuelve 0 o size-1
	inline int get_max_arrays() const {
        return deck_arrays.empty() ? 0 : int(deck_arrays.size()) - 1;
    }
	// Clampea el índice
    void clamp_current_array_index() {
    	current_array_index = std::clamp(current_array_index, 0, get_max_arrays());
	}
};