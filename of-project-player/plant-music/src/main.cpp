// clang-format off
#include <memory>
#include "ofMain.h"
#include "ofxMsgPacketizer.h"
#include "config.hpp"
#include "sounds.hpp"
#include "../../../model.hpp"
// clang-format on 


struct channel_FSM {
    enum {
        idle,
        playing_sound,
    } state;
    // last touched time
    // first touched time
    // need event sound finished to send stop event
};


class ofApp : public ofBaseApp {

    std::unique_ptr<plantmusic::config> config;
    std::unique_ptr<plantmusic::player> player;

  ofSerial serial;

  // {key_i : val_i, key_a : [i, f, s]}
  const uint8_t send_index = 0x12;
  const uint8_t recv_index = 0x34;

  std::stringstream echo_info;
  std::stringstream recv_info;

public:
  void setup() {
    config = std::make_unique<plantmusic::config>();
    player = std::make_unique<plantmusic::player>(config);

    player->setup();
    ofSetVerticalSync(false);
    ofSetFrameRate(120);
    ofSetBackgroundColor(0);

    serial.setup("/dev/ttyUSB0", 115200);

    // publish packet (you can also send function returns)
    //MsgPacketizer::publish(serial, send_index, nested)->setFrameRate(60);

    // handle updated data from arduino
    MsgPacketizer::subscribe(serial, message::ERROR, [&](const message::ERROR_t &n) {
      ofLog()<< "message: error:" << n.message << std::endl;
    });

    MsgPacketizer::subscribe(serial, message::TOUCH_EVENT, [&](const message::TOUCH_EVENT_t &n) {
      player->play_sound(n.channel);
      ofLog() << "TOUCH EVENT on channel " << std::dec << (int)n.channel; 
    });


    // always called if packet has come regardless of index
    MsgPacketizer::subscribe(serial, [&](const uint8_t index,
                                         MsgPack::Unpacker &unpacker) {
      recv_info << "packet has come! index = 0x" << std::hex << (int)index;
      recv_info << ", arg size = " << std::dec << unpacker.size() << std::endl;
    });
  }


  void mousePressed(int x, int y, int button) {
      if (button == 0) {
          player->play_sound(0);
      }
      if (button == 1) {
          player->stop_sound(0);
          player->stop_sound(1);
      }
      if (button == 2) {
          player->play_sound(1);
      }
  }

  void update() {
    recv_info.str("");
    recv_info.clear();
    echo_info.str("");
    echo_info.clear();

    // update publishing data

    // must be called
    MsgPacketizer::update();
    MsgPacketizer::parse();
  }


  void draw() {
    ofDrawBitmapString("FPS : " + ofToString(ofGetFrameRate()), 20, 40);
    ofDrawBitmapString(recv_info.str(), 20, 80);
    ofDrawBitmapString(echo_info.str(), 20, 120);
  }
};

int main() {

  ofSetupOpenGL(480, 360, OF_WINDOW);
  ofRunApp(new ofApp());
}
