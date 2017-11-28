///////////////////////////////////////////////////
//
//   A/B Switches VCV Module
//
//   Strum 2017
//
///////////////////////////////////////////////////

#include "mental.hpp"

#include "dsp/digital.hpp"


/////////////////////////////////////////////////
struct MentalABSwitches : Module {
	enum ParamIds {
      BUTTON_PARAM,
      NUM_PARAMS = BUTTON_PARAM + 4
	};

	enum InputIds {
      INPUT,
      SEL_INPUT = INPUT + 4,
      NUM_INPUTS = SEL_INPUT + 4
	};
	enum OutputIds {
      OUTPUT_A,
      OUTPUT_B = OUTPUT_A + 4,
      NUM_OUTPUTS = OUTPUT_B + 4
	};
  enum LightIds {
		BUTTON_LIGHTS,
    A_LEDS = BUTTON_LIGHTS + 4,
    B_LEDS = A_LEDS + 4,
		NUM_LIGHTS = B_LEDS + 4
	};

  SchmittTrigger button_triggers[4];
  bool button_on[4] = {0,0,0,0};
  
  //float button_lights[4] = {0.0,0.0,0.0};
  //float a_leds[4] = {0.0,0.0,0.0,0.0};
  //float b_leds[4] = {0.0,0.0,0.0,0.0};
  
	MentalABSwitches() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;
  
  json_t *toJson() override
  {
		json_t *rootJ = json_object();
    
    // button states
		json_t *button_statesJ = json_array();
		for (int i = 0; i < 4; i++)
    {
			json_t *button_stateJ = json_integer((int) button_on[i]);
			json_array_append_new(button_statesJ, button_stateJ);
		}
		json_object_set_new(rootJ, "buttons", button_statesJ);    
    return rootJ;
  }
  
  void fromJson(json_t *rootJ) override
  {
    // button states
		json_t *button_statesJ = json_object_get(rootJ, "buttons");
		if (button_statesJ)
    {
			for (int i = 0; i < 4; i++)
      {
				json_t *button_stateJ = json_array_get(button_statesJ, i);
				if (button_stateJ)
					button_on[i] = !!json_integer_value(button_stateJ);
			}
		}  
  }
};

/////////////////////////////////////////////////////
void MentalABSwitches::step() {

  for (int i = 0 ; i < 4 ; i++)
  {
    float signal = inputs[INPUT + i].value;
    float sel = inputs[SEL_INPUT + i].value;
  
    if (button_triggers[i].process(params[BUTTON_PARAM + i].value))
    {
	    button_on[i] = !button_on[i];
    }
    //lights[BLINK_LIGHT].value = (blinkPhase < 0.5) ? 1.0 : 0.0;
    //button_lights[i] = button_on[i] ? 1.0 : 0.0;
    if (button_on[i] || ( sel > 0.0))
    {
      outputs[OUTPUT_A + i].value = 0.0;
      outputs[OUTPUT_B + i].value = signal;
      //b_leds[i] = 1.0;
      //a_leds[i] = 0.0;
      lights[B_LEDS + i].value = 1.0;
      lights[A_LEDS + i].value = 0.0;
    }
    else
    {
      outputs[OUTPUT_A + i].value = signal;
      outputs[OUTPUT_B + i].value = 0.0;
      //b_leds[i] = 0.0;
      //a_leds[i] = 1.0;
      lights[B_LEDS + i].value = 0.0;
      lights[A_LEDS + i].value = 1.0;
    }
  }
}

//////////////////////////////////////////////////////////////////
MentalABSwitchesWidget::MentalABSwitchesWidget() {
	MentalABSwitches *module = new MentalABSwitches();
	setModule(module);
	box.size = Vec(15*5, 380);

  {
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		
    //panel->setBackground(SVG::load("plugins/mental/res/MentalABSwitches.svg"));
    panel->setBackground(SVG::load(assetPlugin(plugin,"res/MentalABSwitches.svg")));
		addChild(panel);
	}
  int group_spacing = 85;
  
  for (int i = 0 ; i < 4 ; i++)
  {
	  addInput(createInput<InPort>(Vec(3, group_spacing * i + 25), module, MentalABSwitches::INPUT + i));
    addInput(createInput<GateInPort>(Vec(3, group_spacing * i + 75), module, MentalABSwitches::SEL_INPUT + i));
  
    addOutput(createOutput<OutPort>(Vec(33, group_spacing * i + 25), module, MentalABSwitches::OUTPUT_A + i));
    addOutput(createOutput<OutPort>(Vec(33, group_spacing * i + 50), module, MentalABSwitches::OUTPUT_B + i));

    addChild(createLight<MediumLight<GreenLight>>(Vec(62, group_spacing * i + 34), module, MentalABSwitches::A_LEDS + i ));
    addChild(createLight<MediumLight<GreenLight>>(Vec(62, group_spacing * i + 59), module, MentalABSwitches::B_LEDS + i));
  
    addParam(createParam<LEDButton>(Vec(6, group_spacing * i + 54), module, MentalABSwitches::BUTTON_PARAM + i, 0.0, 1.0, 0.0));
	  addChild(createLight<MediumLight<GreenLight>>(Vec(6+5, group_spacing * i + 54+5), module, MentalABSwitches::BUTTON_LIGHTS + i));
  }
}
