#include "Card.hpp"
#include <memory>
class MovementController;

class Fireball : public Card /*, public event_system::event_receiver*/ {
public:
	Fireball();
	void on_play(Deck& d,const Vector2D* player_position, const Vector2D* target_position) override;
	//void event_callback0(const Msg& m) override;
};

class Lighting : public Card {
public:
	Lighting();
	void on_play(Deck& d, const Vector2D* player_position, const Vector2D* target_position) override;
};

// Archetypes: Cycle
class Kunai : public Card {
public:
	Kunai();
	void on_play(Deck& d, const Vector2D* player_position, const Vector2D* target_position) override;
};

class Recover : public Card {
public:
	Recover();
	void on_play(Deck& d, const Vector2D* player_position, const Vector2D* target_position) override;
};

// Archetypes: Multi-hit
class Minigun : public Card {
public:
	Minigun();
	void on_play(Deck& d, const Vector2D* player_position, const Vector2D* target_position) override;
	void update(uint32_t) override;
private:
	GameStructs::BulletProperties _bullets_properties;
	const Vector2D* _aim_vec;
	const Vector2D* _pl_vec;
	bool _playing;
	int _time_since_played;
	int _number_of_bullets_shot = 0;
	int _number_of_shots = 10;
	int _shooting_duration = 1500;
	bool _im_primed = false;
};

// Archetypes: Mill
class CardSpray : public Card {
public:
	CardSpray();
	void on_play(Deck& d, const Vector2D* player_position, const Vector2D* target_position) override;
};

// Archetypes: Mill
class EldritchBlast : public Card { // TODO: Reset on round end
public:
	EldritchBlast();
	void on_play(Deck& d, const Vector2D* player_position, const Vector2D* target_position) override;
	Card* on_mill(Deck& d, const Vector2D* player_position) override;
private:
	int _shot_count = 1;
	const int _amplitude = 10;
};

// Archetypes: Mill, Prime
class Primordia : public Card { 
public: 
	Primordia();
	void on_play(Deck& d, const Vector2D* player_position, const Vector2D* target_position) override;
	Card* on_mill(Deck& d, const Vector2D* player_position) override;
	void update(uint32_t) override;
};

// Archetypes: Mill, High Costs
class Commune : public Card {
public:
	Commune();
	void on_play(Deck& d, const Vector2D* player_position, const Vector2D* target_position) override;
};

// Archetypes: Mill, High Costs
class Evoke : public Card {
public:
	Evoke();
	void on_play(Deck& d, const Vector2D* player_position, const Vector2D* target_position) override;

};

//Archetypes: Prime, High Costs
class Fulgur : public Card {
public:
	Fulgur();
	void on_play(Deck& d, const Vector2D* player_position, const Vector2D* target_position) override;
	void update(uint32_t) override;
private:
	GameStructs::BulletProperties _bullets_properties;
	Vector2D _aim_vec;
	bool _playing;
	int _time_since_played;
	int _number_of_bullets_shot = 0;
	int _number_of_shots = 8;
	int _shooting_duration = 500;
};

class CatKuzaCard : public Card {
public:
	CatKuzaCard();
	void on_play(Deck& d, const Vector2D* player_position, const Vector2D* target_position) override;

private:
	int _times_since_played;
};

class SuperMichiCard : public Card {
public:
	SuperMichiCard();
	void on_play(Deck& d, const Vector2D* player_position, const Vector2D* target_position) override;
private:
	int _times_since_played;
};

class QuickFeet : public Card {
public:
	QuickFeet();
	void on_play(Deck& d, const Vector2D* player_position, const Vector2D* target_position) override;
	void update(uint32_t) override;
private:
	bool _playing;
	int _time_since_played;
	int _effect_duration = 1000;
	MovementController* _ctrl;
};

class Arrows : public Card {
public:
    Arrows();
    void on_play(Deck& d, const Vector2D* player_position, const Vector2D* target_position) override;
	void update(uint32_t) override;
private:
	GameStructs::BulletProperties _bullets_properties;
	const Vector2D* _aim_vec;
	const Vector2D* _pl_vec;
	bool _playing;
	int _time_since_played;
	int _number_of_bullets_shot = 0;
	int _number_of_shots = 10;
	int _shooting_duration = 1500;
	bool _im_primed = false;
};

// The following cards exist for testing purposes and may not be playable in the finished product. These are subject to change.
#pragma region TestCards
class Prime : public Card {
public:
	Prime();
	void on_play(Deck& d, const Vector2D* player_position, const Vector2D* target_position) override;
};

// Enum para las cartas jugables
enum class PlayableCardType {
	Fireball,
	Minigun,
	Lighting,
	Kunai,
	Recover,
	CardSpray,
	EldritchBlast,
	Primordia,
	Commune,
	Evoke,
	Fulgur,
	QuickFeet,
	CatKuzaCard,
	Arrows,
	N_CARD_TYPES // Para saber cuántas hay
};

// Creador de cartas a partir del enum
inline Card* create_card_from_type(PlayableCardType type) {
	switch (type) {
		case PlayableCardType::Fireball: return new Fireball();
		case PlayableCardType::Minigun: return new Minigun();
		case PlayableCardType::Lighting: return new Lighting();
		case PlayableCardType::Kunai: return new Kunai();
		case PlayableCardType::Recover: return new Recover();
		case PlayableCardType::CardSpray: return new CardSpray();
		case PlayableCardType::EldritchBlast: return new EldritchBlast();
		case PlayableCardType::Primordia: return new Primordia();
		case PlayableCardType::Commune: return new Commune();
		case PlayableCardType::Evoke: return new Evoke();
		case PlayableCardType::Fulgur: return new Fulgur();
		case PlayableCardType::QuickFeet: return new QuickFeet();
		case PlayableCardType::CatKuzaCard: return new CatKuzaCard();
		case PlayableCardType::Arrows: return new Arrows();
		default: return nullptr;
	}
}

#pragma endregion
