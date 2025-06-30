#pragma once

#include "Scene.h"
#include <vector>
#include <string>
#include <list>

class GameOverScene : public Scene
{
public:
	GameOverScene();
	virtual ~GameOverScene();
	void initScene() override;
	void enterScene() override;
	void exitScene() override;
	void render() override;

	static void set_endless_wave(int wave) { 
		endless_wave = wave+1;
		if (wave+1 > Game::Instance()->getRecord())
			Game::Instance()->setRecord(wave+1);
	}

private:
	void create_enter_button();
	void create_exit_button();

	static int endless_wave;
};