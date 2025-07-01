#include "CustomDeckSelectionScene.h"
#include "../../our_scripts/components/ui/Button.h"
#include "../GameStructs.h"
#include "../../utils/Vector2D.h"
#include "../../sdlutils/SDLUtils.h"
#include "../../sdlutils/InputHandler.h"
#include "../../ecs/Entity.h"

#include "../../our_scripts/components/weapons/player/Revolver.h"
#include "../../our_scripts/components/weapons/player/Rampage.h"
#include "../../our_scripts/components/weapons/player/PumpShotgun.h"
#include "../../our_scripts/components/weapons/player/RampCanon.h"
#include "../../our_scripts/components/weapons/player/Lightbringer.h"

#include "../../our_scripts/card_system/Card.hpp"
#include "../../our_scripts/card_system/PlayableCards.hpp"
#include "../../our_scripts/components/cards/Deck.hpp"
#include "../../our_scripts/components/rendering/Image.h"
#include "../../our_scripts/components/rendering/ImageForButton.h"

#ifdef GENERATE_LOG
#include "../../our_scripts/log_writer_to_csv.hpp"
#endif

#include <iostream>
#include <typeinfo>
#include <algorithm>
#include <memory>
#include <array>
CustomDeckSelectionScene::CustomDeckSelectionScene() 
    : Scene(ecs::scene::DECKSELECTIONMENUSCENE), _weapon_selected(false), _deck_selected(true), _last_weapon_button(nullptr), _last_deck_button(nullptr), _activate_play_button(false), 
    current_array_index(0), mngr(Game::Instance()->get_mngr()) {
}

CustomDeckSelectionScene::~CustomDeckSelectionScene()
{
}
void CustomDeckSelectionScene::create_weapon_buttons() {
    float offsetX = 0.07f;  // Distance between buttons on the X axis
    float startX = 0.71f;   // Starting position of the first button on X
    float startY = 0.2f; // Starting position of the first button on Y

    GameStructs::ButtonProperties buttonPropTemplate = {
        { { startX, startY }, {0.05f, 0.1f} },
        0.0f, "", ecs::grp::WEAPONBUTTON
    };

    //Button back
    GameStructs::ButtonProperties backB = {
        { {0.9f, 0.9f},{0.10f, 0.1f} },
        0.0f, ""
    };
    backB.sprite_key = "back";
    create_back_button(backB);

    GameStructs::ButtonProperties revolverB = buttonPropTemplate;
    revolverB.sprite_key = "revolver_button";
    create_weapon_button(GameStructs::REVOLVER, revolverB);

    buttonPropTemplate.rect.position.x += offsetX;  // Move to the right
    GameStructs::ButtonProperties rampageB = buttonPropTemplate;
    rampageB.sprite_key = "rampage_button";
    create_weapon_button(GameStructs::RAMPAGE, rampageB);

    buttonPropTemplate.rect.position.x += offsetX;  // Move to the right
    GameStructs::ButtonProperties pump_shotgun_B = buttonPropTemplate;
    pump_shotgun_B.sprite_key = "pump_shotgun_button";
    create_weapon_button(GameStructs::PUMP_SHOTGUN, pump_shotgun_B);

    buttonPropTemplate.rect.position.x += offsetX;  // Move to the right
    GameStructs::ButtonProperties rampCanonB = buttonPropTemplate;
    rampCanonB.sprite_key = "ramp_canon_button";
    create_weapon_button(GameStructs::RAMP_CANON, rampCanonB);

    //buttonPropTemplate.rect.position.x += offsetX;  // Move to the right
    //GameStructs::ButtonProperties lightbringerB = buttonPropTemplate;
    //lightbringerB.sprite_key = "lightbringer_button";
    //create_weapon_button(GameStructs::LIGHTBRINGER, lightbringerB);

    //we ll add these two when we have other weapon implemented
    //buttonPropTemplate.rect.position.x += offsetX;  // Move to the right
    //GameStructs::ButtonProperties another_B = buttonPropTemplate;
    //another_B.sprite_key = "arma6_button";
    //create_weapon_button(GameStructs::WEAPON6, another_B);

    //buttonPropTemplate.rect.position.y += umbral; // Move to the next row
    //buttonPropTemplate.rect.position.x = startX;  // Reset X to the initial position
    //buttonPropTemplate.rect.position.x += offsetX;  // Move to the right

    //GameStructs::ButtonProperties another2 = buttonPropTemplate;
    //another2.sprite_key = "arma7_button";
    //create_weapon_button(GameStructs::WEAPON7, another2);
}

void CustomDeckSelectionScene::create_back_button(const GameStructs::ButtonProperties& bp) {
    auto e = create_button(bp);

    auto imgComp = mngr->addComponent<ImageForButton>(e,
        &sdlutils().images().at(bp.sprite_key),
        &sdlutils().images().at(bp.sprite_key + "_selected"),
        bp.rect,
        0,
        mngr->getComponent<camera_component>(mngr->getHandler(ecs::hdlr::CAMERA))->cam
    );

    auto buttonComp = mngr->getComponent<Button>(e);
    buttonComp->connectClick([buttonComp, imgComp, this]() {
        imgComp->_filter = false;
        imgComp->swap_textures();
        Game::Instance()->queue_scene(Game::MAINMENU);});

    buttonComp->connectHover([buttonComp, imgComp]() {
        imgComp->_filter = true;
        imgComp->swap_textures();
        sdlutils().soundEffects().at("button_hover").play();
        });

    buttonComp->connectExit([buttonComp, imgComp]() {
        imgComp->_filter = false;
        imgComp->swap_textures();
        });
}

void CustomDeckSelectionScene::create_deck_buttons() {
    GameStructs::ButtonProperties buttonPropTemplate = {
         { {0.0075f, 0.2f},{0.085f, 0.135f} },
         0.0f, "", ecs::grp::DECKBUTTON
    };
    
    GameStructs::ButtonProperties deck1B = buttonPropTemplate;
    deck1B.sprite_key = "deck1_button";
    create_deck_button(GameStructs::ONE, deck1B);

    buttonPropTemplate.rect.position.x += 0.05f;
    GameStructs::ButtonProperties deck2B = buttonPropTemplate;
    deck2B.sprite_key = "deck2_button";
    create_deck_button(GameStructs::TWO, deck2B);

    buttonPropTemplate.rect.position.x += 0.05f;
    GameStructs::ButtonProperties deck3B = buttonPropTemplate;
    deck3B.sprite_key = "deck3_button";
    create_deck_button(GameStructs::THREE, deck3B);

    buttonPropTemplate.rect.position.x += 0.05f;
    GameStructs::ButtonProperties deck4B = buttonPropTemplate;
    deck4B.sprite_key = "deck4_button";
    create_deck_button(GameStructs::FOUR, deck4B);
}

void CustomDeckSelectionScene::initScene() {
    create_static_background(&sdlutils().images().at("selection_deck"));
    create_weapon_info();
    create_weapon_buttons();
    create_enter_button();
    create_deck_buttons(); // Botones de decks

    create_card_buttons(); // Mostrar todas las cartas como botones
    create_remove_card_button(); // Botón para eliminar carta

    create_arrow_buttons(); // Botones de flechas para cambiar de página
}

void CustomDeckSelectionScene::reset() {
    _weapon_selected = false;
    //we need to clean the filter of both side texture
    if (_last_deck_button != nullptr) {
        _last_deck_button->_filter = false;
        _last_deck_button->swap_textures();
        _last_deck_button->_filter = false;
    }
    _last_deck_button = nullptr;
    if (_last_weapon_button != nullptr) {
        _last_weapon_button->_filter = false;
        _last_weapon_button->swap_textures();
        _last_weapon_button->_filter = false;
    }
    _last_weapon_button = nullptr;
    _activate_play_button = false;

    auto& deckInfo = mngr->getEntities(ecs::grp::DECKINFO);
    for (auto& d : deckInfo) {
        auto img = mngr->getComponent<transformless_dyn_image>(d);
        img->set_texture(&sdlutils().images().at("initial_info"));
    }
    assign_default_deck();
}

void CustomDeckSelectionScene::enterScene()
{
    Game::Instance()->get_mngr()->change_ent_scene(Game::Instance()->get_mngr()->getHandler(ecs::hdlr::CAMERA), ecs::scene::DECKSELECTIONMENUSCENE);
#ifdef GENERATE_LOG
    log_writer_to_csv::Instance()->add_new_log();
    log_writer_to_csv::Instance()->add_new_log("ENTERED DECK SELECTION MENU SCENE");
#endif
    reset();
    if (!Game::Instance()->is_network_none()) {
        Game::network_users_state& state = Game::Instance()->get_network_state();
        state.game_state.ready_users.reset();
        state.game_state.ready_user_count = 0;
    }
}

void CustomDeckSelectionScene::exitScene()
{
    //if(_activate_play_button)mngr->getComponent<ImageForButton>(mngr->getHandler(ecs::hdlr::TOGAMEBUTTON))->swap_textures();

    auto playB = mngr->getHandler(ecs::hdlr::TOGAMEBUTTON);
    auto playImg = mngr->getComponent<ImageForButton>(playB);
    playImg->_filter = false;
    sdlutils().musics().at("main_menu_bgm").haltMusic();
    sdlutils().musics().at("game_bgm").play();

    auto imgComp = mngr->getComponent<ImageForButton>(mngr->getHandler(ecs::hdlr::TOGAMEBUTTON));
    imgComp->set_texture(&sdlutils().images().at("ready"));
    imgComp->destination_rect.position.x = 10.4f;


#ifdef GENERATE_LOG
    log_writer_to_csv::Instance()->add_new_log("EXIT SELECTION MENU SCENE");
    log_writer_to_csv::Instance()->add_new_log();
#endif

}

void CustomDeckSelectionScene::render()
{
    Scene::render();
    if (showing_message) {
        auto camera = Game::Instance()->get_mngr()->getComponent<camera_component>(Game::Instance()->get_mngr()->getHandler(ecs::hdlr::CAMERA));
        rect_f32 message_rect = rect_f32_screen_rect_from_viewport(rect_f32({ { 0.4,0.7 }, { 0.2,0.05 } }), camera->cam.screen);
        SDL_Rect message_true{
            int(message_rect.position.x),
            int(message_rect.position.y),
            int(message_rect.size.x),
            int(message_rect.size.y)
        };
        Texture message_tex{
        sdlutils().renderer(),
        message,
        sdlutils().fonts().at("RUBIK_MONO"),
        SDL_Color({128, 0, 32, 255}) };
        message_tex.render(message_true);
    }
}

void CustomDeckSelectionScene::create_weapon_button(GameStructs::WeaponType wt, const GameStructs::ButtonProperties& bp) {
    auto e = create_button(bp);
    auto buttonComp = mngr->getComponent<Button>(e);
    //used for change the sprite once a button is clicked
    auto imgComp = mngr->addComponent<ImageForButton>(e,
        &sdlutils().images().at(bp.sprite_key),
        &sdlutils().images().at(bp.sprite_key + "_selected"),
        bp.rect,
        0,
        Game::Instance()->get_mngr()->getComponent<camera_component>(
            Game::Instance()->get_mngr()->getHandler(ecs::hdlr::CAMERA))->cam
    );
    auto player = mngr->getHandler(ecs::hdlr::PLAYER);

    buttonComp->connectClick([buttonComp, imgComp, wt, player, this]() {
        
        std::string s;

        switch (wt) {
        case GameStructs::REVOLVER:
            mngr->addComponent<Revolver>(player);
            break;
        case GameStructs::RAMPAGE:
            mngr->addComponent<Rampage>(player);
            break;
        case GameStructs::PUMP_SHOTGUN:
            mngr->addComponent<PumpShotgun>(player);
            break;
        case GameStructs::RAMP_CANON:
            mngr->addComponent<RampCanon>(player);
            break;
        case GameStructs::LIGHTBRINGER:
            mngr->addComponent<Lightbringer>(player);
            break;
        case GameStructs::WEAPON6:
            mngr->addComponent<Rampage>(player);
            break;
        case GameStructs::WEAPON7:
            mngr->addComponent<Revolver>(player);
            break;
        default:
            break;
        }

        _weapon_selected = true;

        if (imgComp != _last_weapon_button) {
            //swap the actual buttons textures
            imgComp->_filter = false;
            imgComp->swap_textures();
        }

        //register the last clicked button
        if (_last_weapon_button != nullptr && _last_weapon_button != imgComp) {
            _last_weapon_button->swap_textures();
        }
        _last_weapon_button = imgComp;
        

    });
    buttonComp->connectHover([buttonComp, imgComp, this, wt]() {
        sdlutils().soundEffects().at("button_hover").play();
        std::string s;
        auto info = mngr->getHandler(ecs::hdlr::WEAPONINFO);
        auto infoImg = mngr->getComponent<transformless_dyn_image>(info);
        switch (wt) {
        case GameStructs::REVOLVER:
            s = "revolver_";
            break;
        case GameStructs::RAMPAGE:
            s = "rampage_";
            break;
        case GameStructs::PUMP_SHOTGUN:
            s = "pump_shotgun_";
            break;
        case GameStructs::RAMP_CANON:
            s = "ramp_canon_";
            break;
        case GameStructs::LIGHTBRINGER:
            s = "lightbringer_";
            break;
        case GameStructs::WEAPON6:
            s = "arma6_";
            break;
        case GameStructs::WEAPON7:
            s = "arma7_";
            break;
        default:
            break;
        }
        s += "info";
        infoImg->set_texture(&sdlutils().images().at(s));
        imgComp->_filter = true;
    });
    buttonComp->connectExit([buttonComp, imgComp, this]() {
        auto info = mngr->getHandler(ecs::hdlr::WEAPONINFO); 
        auto infoImg = mngr->getComponent<transformless_dyn_image>(info); 
        infoImg->set_texture(&sdlutils().images().at("initial_info"));
        imgComp->_filter = false;
    });
}

void CustomDeckSelectionScene::create_deck_button(GameStructs::DeckType dt, const GameStructs::ButtonProperties& bp) {
    auto e = create_button(bp);
    auto buttonComp = mngr->getComponent<Button>(e);
    auto player = mngr->getHandler(ecs::hdlr::PLAYER);
    //used for change the sprite once a button is clicked
    auto imgComp = mngr->addComponent<ImageForButton>(e,
        &sdlutils().images().at(bp.sprite_key),
        &sdlutils().images().at(bp.sprite_key + "_selected"),
        bp.rect,
        0,
        mngr->getComponent<camera_component>(mngr->getHandler(ecs::hdlr::CAMERA))->cam
    );


    buttonComp->connectClick([buttonComp, imgComp, player, dt, this]() {
        
        assign_default_deck();
        
        switch (dt)
        {
        case GameStructs::ONE:
            add_card_to_deck(new Evoke());
            add_card_to_deck(new Fireball());
            add_card_to_deck(new Minigun());
            add_card_to_deck(new Commune());
            break;
        case GameStructs::TWO:
            add_card_to_deck(new Fireball());
            add_card_to_deck(new CardSpray());
            add_card_to_deck(new Lighting());
            add_card_to_deck(new Minigun());
            break;
        case GameStructs::THREE:
            add_card_to_deck(new Boomerang());
            add_card_to_deck(new Fireball());
            add_card_to_deck(new Minigun());
            add_card_to_deck(new Arrows());
            break;
        case GameStructs::FOUR:
            add_card_to_deck(new Kunai());
            add_card_to_deck(new EldritchBlast());
            add_card_to_deck(new QuickFeet());
            add_card_to_deck(new CardSpray());
            break;
        default:
            break;
        }
        _deck_selected = true;

        //swap the actual buttons textures
        if (_last_deck_button != nullptr && _last_deck_button != imgComp) {
            imgComp->_filter = false;
            imgComp->swap_textures();
            _last_deck_button->swap_textures();
            //register the clicked button
        }
        else if (_last_deck_button == nullptr) { //special case: first click
            imgComp->_filter = false;
            imgComp->swap_textures();
        }
        _last_deck_button = imgComp;

        refresh_deck_visual(0); // Actualizar visualización del mazo
    });
    buttonComp->connectHover([buttonComp, imgComp]() {
        imgComp->_filter = true;
        sdlutils().soundEffects().at("button_hover").play();
    });
    buttonComp->connectExit([buttonComp, imgComp]() {
        imgComp->_filter = false;
    });
}

void CustomDeckSelectionScene::create_weapon_info() {
   // rect_f32 rect = {{1.3f, 0.25f} ,{0.75f, 0.5f}};
    rect_f32 rect = { {0.695f, 0.315f} ,{0.3f, 0.25f} };
    ecs::entity_t e = create_entity(
        ecs::grp::UI,
        ecs::scene::DECKSELECTIONMENUSCENE,
        new transformless_dyn_image
        (rect,
            0,
            mngr->getComponent<camera_component>(mngr->getHandler(ecs::hdlr::CAMERA))->cam,
            &sdlutils().images().at("initial_info"))
    );
    mngr->setHandler(ecs::hdlr::WEAPONINFO, e);
}

// Organizamos el mazo visible en arrays de 4 elementos
void CustomDeckSelectionScene::organize_deck_into_arrays() {
    auto deckComp = mngr->getComponent<Deck>(mngr->getHandler(ecs::hdlr::PLAYER));
    if (!deckComp) return;
    auto& cl = deckComp->get_draw_pile();


    // Vacía para empezar (se resetea cada vez)
    deck_arrays.clear();
    
    // Si no hay cartas, crear un array vacío
    if (cl.empty()) {
        std::array<Card*, CARDS_PER_ARRAY> empty_array = {nullptr, nullptr, nullptr, nullptr};
        deck_arrays.push_back(empty_array);
        return;
    }

    // Organizar cartas en arrays de 4
    auto it = cl.begin();
    while (it != cl.end()) {

        // Array actual
        std::array<Card*, CARDS_PER_ARRAY> current_array = {nullptr, nullptr, nullptr, nullptr};
        
        // Llenar array actual hasta 4 o hasta que se acaben
        for (int i = 0; i < CARDS_PER_ARRAY && it != cl.end(); ++i) {
            current_array[i] = *it++; // Asigna y avanza
        }
        
        // Añadir al vector el array de cartas
        deck_arrays.push_back(current_array);
    }
}

// Mostramos el mazo visible
void CustomDeckSelectionScene::update_visual_display() {
    
    // Primero clampeamos el índice
    clamp_current_array_index();

    // Obtener el array actual a mostrar
    std::array<Card*, CARDS_PER_ARRAY> current_array = {nullptr, nullptr, nullptr, nullptr};
    if (!deck_arrays.empty()) {
        current_array = deck_arrays[current_array_index];
    }
    
    // Resetea todas las entidades de info
    auto& infos = mngr->getEntities(ecs::grp::DECKINFO);
    for (auto& info : infos) {
        mngr->setAlive(info, false);
    }
    
    // Resetear posición inicial
    display_rect = or_display_rect;
    
    // Creamos 4 entidades
    for (int i = 0; i < CARDS_PER_ARRAY; ++i) {
        ecs::entity_t e = create_entity(
            ecs::grp::DECKINFO,
            ecs::scene::DECKSELECTIONMENUSCENE,
            new transformless_dyn_image(display_rect, 0,
                mngr->getComponent<camera_component>(mngr->getHandler(ecs::hdlr::CAMERA))->cam,
                &sdlutils().images().at("initial_info"))
        );
        
        // Si existe la carta, se muestra ahí
        if (current_array[i] != nullptr) {
            auto img = mngr->getComponent<transformless_dyn_image>(e);
            std::string typeName = current_array[i]->get_name();
            img->set_texture(&sdlutils().images().at(typeName + "_info"));
        }
        
        display_rect.position.y += umbral;
    }
}

// Función principal que actualiza todo el display
void CustomDeckSelectionScene::refresh_deck_visual(int actualizar_index = -1) {
    // Si se le pasa un argumento de índice, se muestra el display en ese índice
    // Por ejemplo, al añadir/eliminar cartas, se va a la última página
    current_array_index = (actualizar_index == -1) ? current_array_index : actualizar_index;
    organize_deck_into_arrays();
    update_visual_display();
}

void CustomDeckSelectionScene::create_enter_button() {
    GameStructs::ButtonProperties bp = {
         { {10.0f, 0.18f},{0.2f, 0.15f} },
         0.0f, "ready", ecs::grp::UI
    };
    auto e = create_button(bp);
    mngr->setHandler(ecs::hdlr::TOGAMEBUTTON, e);
    auto imgComp = mngr->addComponent<ImageForButton>(e,
        &sdlutils().images().at("initial_info"),
        &sdlutils().images().at(bp.sprite_key+"_selected"),
        bp.rect,
        0,
        mngr->getComponent<camera_component>(mngr->getHandler(ecs::hdlr::CAMERA))->cam
    );


    auto buttonComp = mngr->getComponent<Button>(e);

    buttonComp->connectClick([buttonComp, imgComp, this]() {
        if (_weapon_selected /*&& _deck_selected*/) { // Solo depende del arma
            if (!Game::Instance()->is_network_none()) {
                network_context& network = Game::Instance()->get_network();
                Game::network_users_state& state = Game::Instance()->get_network_state();
                uint32_t id = state.connections.local_user_index;
                if (Game::Instance()->is_host()) {
                    state.game_state.ready_users.set(0, true);
                    state.game_state.ready_user_count++;
                    if (checkAllPlayersReady())Game::Instance()->queue_scene(Game::GAMESCENE); // cambiar a donde inicializamos!!
                }
                else if(!state.game_state.ready_users.test(id)){
                    state.game_state.ready_users.set(id, true);

                    network_message_pack_send(
                        network.profile.client.socket_to_host,
                        network_message_pack_create(network_message_type_player_ready,
                            create_player_id_message(id))
                    );
                }
                show_message("Esperando a los otros michis...", 5 * 1000);
            }
            else {
                imgComp->_filter = false;
                // delete_default_card(); // Eliminaba la carta por defecto del mazo
                Game::Instance()->queue_scene(Game::ENDLESSGAMESCENE);
                imgComp->destination_rect.position.x = 10.0f;
            }
        }
    });
    buttonComp->connectHover([buttonComp, imgComp, this]() {     
        if (!_activate_play_button) return;
        sdlutils().soundEffects().at("button_hover").play(); 
        imgComp->_filter = true;
        imgComp->swap_textures();
        });
    buttonComp->connectExit([buttonComp, imgComp, this]() { 
        if (!_activate_play_button) return;
        imgComp->_filter = false;
    imgComp->swap_textures();});
}

void CustomDeckSelectionScene::update(uint32_t delta_time) {

    Scene::update(delta_time);

    if (!_activate_play_button && _last_weapon_button /*&& _last_deck_button*/) { // Solo depende del arma
        auto imgComp = mngr->getComponent<ImageForButton>(mngr->getHandler(ecs::hdlr::TOGAMEBUTTON));
        imgComp->set_texture(&sdlutils().images().at("ready"));
        _activate_play_button = true;
        imgComp->destination_rect.position.x = 0.4f;
    }

    if (!Game::Instance()->is_network_none()) {
        network_context& network = Game::Instance()->get_network();

        if (Game::Instance()->is_host()) {
            if (SDLNet_CheckSockets(network.profile.host.clients_host_set, 0) > 0) {
                for (network_connection_size i = 0; i < network.profile.host.sockets_to_clients.connection_count; ++i) {
                    TCPsocket& connection = network.profile.host.sockets_to_clients.connections[i];
                    if (SDLNet_SocketReady(connection)) {
                        network_message_dynamic_pack dyn_message = network_message_dynamic_pack_receive(connection);
                        const uint16_t type_n{ dyn_message->header.type_n };
                        const uint16_t type_h{ SDLNet_Read16(&type_n) };
                        switch (type_h) {
                        case network_message_type_player_ready: {
                            auto message = network_message_dynamic_pack_into<network_message_player_id>(std::move(dyn_message));
                            auto&& payload = message->payload.content;
                            uint32_t id = SDLNet_Read32(&payload.id_n);

                            Game::network_users_state& state = Game::Instance()->get_network_state();
                            state.game_state.ready_users.set(id, true);
                            state.game_state.ready_user_count++;

                            if (checkAllPlayersReady())Game::Instance()->queue_scene(Game::ENDLESSGAMESCENE);
                            break;
                        }
                        default: {
                            break;
                        }
                        }
                    }
                }
            }
        }
        else {
            int active_sockets = SDLNet_CheckSockets(network.profile.client.client_set, 0);
            if (active_sockets > 0 && SDLNet_SocketReady(network.profile.client.socket_to_host)) {
                auto msg = network_message_dynamic_pack_receive(network.profile.client.socket_to_host);
                const uint16_t type_n{ msg->header.type_n };
                const uint16_t type_h{ SDLNet_Read16(&type_n) };
                switch (type_h) {
                case network_message_type_start_game: {
                    std::cout << "mensaje de ir al juego" << std::endl;
                    Game::Instance()->queue_scene(Game::ENDLESSGAMESCENE);
                    break;
                }
                default: break;
                }
            }

        }
       
    }
}

// Crea un botón por carta y las pone en el medio en dos columnas
void CustomDeckSelectionScene::create_card_buttons() {
    float startX = 0.48f; // columna 1
    float startY = 0.35f; // fila 1
    float offsetX = 0.07f; // offset entre columnas
    float offsetY = 0.1f; // offset entre filas
    int nColumns = 3;
    int idx = 0;

    // Vector con todas las cartas en el enum PlayableCardType
    std::vector<PlayableCardType> all_card_types;
    for (int i = 0; i < int(PlayableCardType::N_CARD_TYPES); i++) {
        all_card_types.push_back(PlayableCardType(i));
    }

    // Crear un botón para cada carta
    for (PlayableCardType cardType : all_card_types) {
        Card* card = create_card_from_type(cardType);
        if (!card) continue;
        
        // POSICIÓN
        float x = startX + (idx % nColumns) * offsetX; // columna
        float y = startY + (idx / nColumns) * offsetY; // fila
        ++idx;

        // BOTÓN DE LA CARTA
        GameStructs::ButtonProperties bp = {
            { {x, y}, {0.05f, 0.08f} },
            0.0f, card->get_name(), ecs::grp::UI
        };
        auto e = create_button(bp);
        auto imgComp = mngr->addComponent<ImageForButton>(e,
            &sdlutils().images().at(card->get_name()),
            &sdlutils().images().at(card->get_name()),
            bp.rect,
            0,
            mngr->getComponent<camera_component>(mngr->getHandler(ecs::hdlr::CAMERA))->cam
        );

        // CONNECTS
        auto buttonComp = mngr->getComponent<Button>(e);
        // Se le pasa el tipo de carta para crear una nueva instancia en el mazo
        buttonComp->connectClick([this, cardType]() {
            Card* newCard = create_card_from_type(cardType);
            if (newCard) add_card_to_deck(newCard);
        });
        buttonComp->connectHover([imgComp]() {
            imgComp->_filter = true;
            sdlutils().soundEffects().at("button_hover").play();
        });
        buttonComp->connectExit([imgComp]() { imgComp->_filter = false; });

        delete card; // Limpiamos la carta creada
        // He intentado hacerlo con unique/shared ptrs pero había que liberarlos después y era más lioso
    }
}

// Antes: Añade una carta al antepenúltimo puesto y mueve la Default al final
// Ahora: Añade una carta al final del mazo
void CustomDeckSelectionScene::add_card_to_deck(Card* card) {
    auto deckComp = mngr->getComponent<Deck>(mngr->getHandler(ecs::hdlr::PLAYER));
    if (!deckComp) return;
    auto& cl = deckComp->get_draw_pile();

    // if (cl.size() <= 1) {
    //     cl.push_back(card);
    // } else {
    //     auto ult_card = cl.back();
    //     cl.pop_back();
    //     cl.push_back(card);
    //     cl.push_back(ult_card); 
    // }

    cl.push_back(card); // Añadir la carta al final del mazo

    refresh_deck_visual(get_max_arrays()+1);
}

// Crea un botón para eliminar la última carta del mazo
void CustomDeckSelectionScene::create_remove_card_button() {
    GameStructs::ButtonProperties bp = {
        { {0.525f, 0.83f}, {0.15f, 0.15f} },
        0.0f, "remove_card", ecs::grp::UI
    };

    auto e = create_button(bp);
    auto imgComp = mngr->addComponent<ImageForButton>(e,
        &sdlutils().images().at("remove_card"),
        &sdlutils().images().at("remove_card_selected"),
        bp.rect,
        0,
        mngr->getComponent<camera_component>(mngr->getHandler(ecs::hdlr::CAMERA))->cam
    );

    auto buttonComp = mngr->getComponent<Button>(e);
    buttonComp->connectClick([this]() {
        remove_last_card_from_deck();
    });
    buttonComp->connectHover([imgComp]() {
        imgComp->_filter = true;
        sdlutils().soundEffects().at("button_hover").play();
    });
    buttonComp->connectExit([imgComp]() {
        imgComp->_filter = false;
    });
}

// Antes: Elimina la última carta antes de la Default y mantiene la Default al final
// Ahora: Elimina la última carta del mazo
void CustomDeckSelectionScene::remove_last_card_from_deck() {
    auto deckComp = mngr->getComponent<Deck>(mngr->getHandler(ecs::hdlr::PLAYER));
    if (!deckComp) return;
    auto& cl = deckComp->get_draw_pile();
    
    // if (cl.size() > 1) {
    //     // Guardar default y eliminarla
    //     auto ult_card = cl.back();
    //     cl.pop_back();
        
    //     // Guardar penult
    //     auto penult_card = cl.back();
    //     cl.pop_back();

    //     // Eliminar penult
    //     // deckComp->remove_card(penult_card); // Esto usa remove y no nos sirve
    //     delete penult_card;

    //     // Volver a añadir default
    //     cl.push_back(ult_card); 
    // }
    
    if (!cl.empty()) {
        delete cl.back();
        cl.pop_back();
    }
    
    refresh_deck_visual(get_max_arrays());
}


// Asigna el mazo por defecto vacío
void CustomDeckSelectionScene::assign_default_deck() {
    auto player = mngr->getHandler(ecs::hdlr::PLAYER);

    std::list<Card*> cl = {  }; // Antes: Kunai como Default y se eliminaba al entrar a partida

    // Crear Deck por primera vez
    auto deckComp = mngr->getComponent<Deck>(player);
    if (!deckComp) {
        deckComp = mngr->addComponent<Deck>(player, cl);
    }
    deckComp->get_draw_pile() = cl;
    
    refresh_deck_visual(0);
}

// Se usaba antes para eliminar la carta Default del mazo antes de entrar a partida
// void CustomDeckSelectionScene::delete_default_card() {
//     auto deckComp = mngr->getComponent<Deck>(mngr->getHandler(ecs::hdlr::PLAYER));
//     if (!deckComp) return;
//     auto& cl = deckComp->get_draw_pile();

//     if (cl.size() > 1) {
//         delete cl.back();
//         cl.pop_back();
//     }

//     refresh_deck_visual(cl);
// }

void CustomDeckSelectionScene::create_arrow_buttons() {

    // Flecha arriba
    GameStructs::ButtonProperties up_bp = {
        { {0.40f, 0.45f}, {0.075f, 0.15f} },
        0.0f, "arrow_up", ecs::grp::UI
    };
    auto up_e = create_button(up_bp);
    auto up_img = mngr->addComponent<ImageForButton>(up_e,
        &sdlutils().images().at("arrow_up"),
        &sdlutils().images().at("arrow_up_selected"),
        up_bp.rect, 0,
        mngr->getComponent<camera_component>(mngr->getHandler(ecs::hdlr::CAMERA))->cam
    );
    auto up_btn = mngr->getComponent<Button>(up_e);
    up_btn->connectClick([this]() { change_page(-1); });
    up_btn->connectHover([up_img]() { up_img->_filter = true; });
    up_btn->connectExit([up_img]() { up_img->_filter = false; sdlutils().soundEffects().at("button_hover").play();});

    // Flecha abajo
    GameStructs::ButtonProperties down_bp = {
        { {0.40f, 0.65f}, {0.075f, 0.15f} },
        0.0f, "arrow_down", ecs::grp::UI
    };
    auto down_e = create_button(down_bp);
    auto down_img = mngr->addComponent<ImageForButton>(down_e,
        &sdlutils().images().at("arrow_down"),
        &sdlutils().images().at("arrow_down_selected"),
        down_bp.rect, 0,
        mngr->getComponent<camera_component>(mngr->getHandler(ecs::hdlr::CAMERA))->cam
    );
    auto down_btn = mngr->getComponent<Button>(down_e);
    down_btn->connectClick([this]() { change_page(1); });
    down_btn->connectHover([down_img]() { down_img->_filter = true; });
    down_btn->connectExit([down_img]() { down_img->_filter = false; sdlutils().soundEffects().at("button_hover").play();});
}