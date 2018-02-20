///////////////////////////////////////////////////
//
//   
//   Gate maker, make gates by clock - VCV Module
//
//   Strum 2017
//
///////////////////////////////////////////////////

#include "mental.hpp"

#include "dsp/digital.hpp"

#include <sstream>
#include <iomanip>

struct MentalGateMaker : Module {
	enum ParamIds {
    RST_BUTTON,
    TRIG_BUTTON,
    CYCLE_BUTTON,
    COUNT_NUM_PARAM,
    DELAY_PARAM,
    TAIL_PARAM,
		NUM_PARAMS
	};  
	enum InputIds {
    CLK_IN,
   	RESET_IN,
    TRIGGER_IN,
    CYCLE_IN,
    COUNT_CV,
    DELAY_CV,
    TAIL_CV,	  
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT,
    FINISH_OUT,
    TRIG_OUT,    
		NUM_OUTPUTS
	};
  enum LightIds {
		CYCLE_BUTTON_LED,
		NUM_LIGHTS
	};
  
  SchmittTrigger clock_trigger;
  SchmittTrigger reset_trigger;
  SchmittTrigger cycle__button_trigger;
  SchmittTrigger trigger_trigger;
  
  //PulseGenerator end;
  //float pulse_length = 0.05; 
  
  int count_limit = 0;
  int count_on = 0;
  int count_disp = 0;
  int count = 0;
  int delay = 0;
  int tail = 0;
  bool cycle = true;
  bool cycle_button_state = true;
  
  bool triggered = false;
  //float cycle_button_led = 0.0;
  bool mode = false;  
    
  MentalGateMaker();//   : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;
};

MentalGateMaker::MentalGateMaker()
{
  params.resize(NUM_PARAMS);
	inputs.resize(NUM_INPUTS);
	outputs.resize(NUM_OUTPUTS);
  lights.resize(NUM_LIGHTS);
	//clock_trigger.setThresholds(0.0, 1.0);
	//reset_trigger.setThresholds(0.0, 1.0);  
}

void MentalGateMaker::step()
{
  
  
  if (cycle__button_trigger.process(params[CYCLE_BUTTON].value))
  {
    cycle_button_state = !cycle_button_state;
    count = 0;
    outputs[OUTPUT].value = 0;
    outputs[FINISH_OUT].value = 0.0; 
  }    
  
  if (inputs[CYCLE_IN].value || cycle_button_state)
  {
    cycle = true;
    //cycle_button_led = 1.0;
    lights[CYCLE_BUTTON_LED].value = 1.0;
    //count = 0;
    //outputs[OUTPUT].value = 0;
    
  } else
  { 
    cycle = false;
    //cycle_button_led = 0.0;
    lights[CYCLE_BUTTON_LED].value = 0.0;
    
  }
  delay = round(params[DELAY_PARAM].value); 
  count_on = round(params[COUNT_NUM_PARAM].value);
  count_disp = count_on + 1;
  tail = round(params[TAIL_PARAM].value);
  count_limit = count_on + delay + tail;
  
  bool reset = false;
    
  if (trigger_trigger.process(params[TRIG_BUTTON].value)  || (trigger_trigger.process(inputs[TRIGGER_IN].value)))
	{
    reset = true;
    triggered = true;
    count = 0;    
    outputs[OUTPUT].value = 0;
    outputs[FINISH_OUT].value = 0.0; 
  }
  
  if (reset_trigger.process(params[RST_BUTTON].value)  || (reset_trigger.process(inputs[RESET_IN].value)))
	{
    reset = true;
    count = 0;
    outputs[OUTPUT].value = 0;
    outputs[FINISH_OUT].value = 0.0;     
  }
  //if (count == 0) outputs[FINISH_OUT].value = 0.0;   
  if (reset == false && (cycle || triggered))
	{
		if (clock_trigger.process(inputs[CLK_IN].value) && count <= count_limit)
					count++;	
  }
  
  if (count >= delay && count <= (count_on + delay)) outputs[OUTPUT].value = 10.0;
  else outputs[OUTPUT].value = 0.0;
  if (count > count_limit)
  {
    count = 0;
    if (triggered) outputs[OUTPUT].value = 0;
    triggered = false;        
    outputs[FINISH_OUT].value = 10.0; 
  }
  
  //outputs[FINISH_OUT].value = end.process(1.0/engineGetSampleRate) ? 5.0 : 0.0;
}

////////////////////////////////////

struct NumberDisplayWidget : TransparentWidget {

  int *value;
  std::shared_ptr<Font> font;

  NumberDisplayWidget() {
    font = Font::load(assetPlugin(plugin, "res/Segment7Standard.ttf"));
  };

  void draw(NVGcontext *vg) override
  {
    // Background
    NVGcolor backgroundColor = nvgRGB(0x00, 0x00, 0x00);
    nvgBeginPath(vg);
    nvgRoundedRect(vg, 0.0, 0.0, box.size.x, box.size.y, 4.0);
    nvgFillColor(vg, backgroundColor);
    nvgFill(vg);    
    
    // text 
    nvgFontSize(vg, 18);
    nvgFontFaceId(vg, font->handle);
    nvgTextLetterSpacing(vg, 2.5);

    std::stringstream to_display;   
    to_display << std::setw(3) << *value;

    Vec textPos; // = Vec(0, 0);   
    NVGcolor textColor = nvgRGB(0x33, 0x33, 0xff);
    nvgFillColor(vg, textColor);
    nvgText(vg, textPos.x, textPos.y, to_display.str().c_str(), NULL);
  }
};

//////////////////////////////////
struct MentalGateMakerWidget : ModuleWidget {
  MentalGateMakerWidget(MentalGateMaker *module);
};

MentalGateMakerWidget::MentalGateMakerWidget(MentalGateMaker *module) : ModuleWidget(module)
{
//MentalGateMakerWidget::MentalGateMakerWidget() {
//	MentalGateMaker *module = new MentalGateMaker();
//	setModule(module);


	box.size = Vec(15*8, 380);
  
	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		
    panel->setBackground(SVG::load(assetPlugin(plugin,"res/MentalGateMaker.svg")));
		addChild(panel);
	}
  /*RST_BUTTON,
    TRIG_BUTTON,
    MODE_BUTTON,
    CYCLE_BUTTON,
    COUNT_NUM_PARAM,
    DELAY_PARAM,
    TAIL_PARAM
		
    CLK_IN,
   	RESET_IN,
    TRIGGER_IN,
    CYCLE_IN,
    COUNT_CV,
    DELAY_CV,*/
	
  addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(2, 20), module, MentalGateMaker::COUNT_NUM_PARAM, 0.0, 32.0, 0.0));
  addInput(Port::create<CVInPort>(Vec(33, 20), Port::INPUT, module, MentalGateMaker::COUNT_CV));
  NumberDisplayWidget *count_display = new NumberDisplayWidget();
	count_display->box.pos = Vec(63,20);
	count_display->box.size = Vec(50, 20);
	count_display->value = &module->count_disp;
	addChild(count_display);   
  
  addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(2, 50), module, MentalGateMaker::DELAY_PARAM, 0.0, 32.0, 0.0)); 
  addInput(Port::create<CVInPort>(Vec(33, 50), Port::INPUT, module, MentalGateMaker::DELAY_CV));
  NumberDisplayWidget *delay_display = new NumberDisplayWidget();
	delay_display->box.pos = Vec(63,50);
	delay_display->box.size = Vec(50, 20);
	delay_display->value = &module->delay;
	addChild(delay_display);
  
  addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(2, 80), module, MentalGateMaker::TAIL_PARAM, 0.0, 32.0, 0.0)); 
  addInput(Port::create<CVInPort>(Vec(33, 80), Port::INPUT, module, MentalGateMaker::TAIL_CV));
  NumberDisplayWidget *tail_display = new NumberDisplayWidget();
	tail_display->box.pos = Vec(63,80);
	tail_display->box.size = Vec(50, 20);
	tail_display->value = &module->tail;
	addChild(tail_display);      
  
  int offset = 30;
  
  addInput(Port::create<GateInPort>(Vec(3, 80 + offset), Port::INPUT, module, MentalGateMaker::CLK_IN));
	addInput(Port::create<GateInPort>(Vec(3, 110 + offset), Port::INPUT, module, MentalGateMaker::RESET_IN));
  addParam(ParamWidget::create<LEDButton>(Vec(35, 110 + offset), module, MentalGateMaker::RST_BUTTON, 0.0, 1.0, 0.0));
  addInput(Port::create<GateInPort>(Vec(3, 140 + offset), Port::INPUT, module, MentalGateMaker::TRIGGER_IN));
  addParam(ParamWidget::create<LEDButton>(Vec(35, 140 + offset), module, MentalGateMaker::TRIG_BUTTON, 0.0, 1.0, 0.0));  
  
  addParam(ParamWidget::create<LEDButton>(Vec(35, 170 + offset), module, MentalGateMaker::CYCLE_BUTTON, 0.0, 1.0, 0.0));
  addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(35+5, 170 + offset + 5), module, MentalGateMaker::CYCLE_BUTTON_LED));
  addInput(Port::create<GateInPort>(Vec(3, 170 + offset), Port::INPUT, module, MentalGateMaker::CYCLE_IN));  
  
  addOutput(Port::create<GateOutPort>(Vec(93, 110), Port::OUTPUT, module, MentalGateMaker::OUTPUT)); 
  addOutput(Port::create<GateOutPort>(Vec(93, 140), Port::OUTPUT, module, MentalGateMaker::TRIG_OUT));
  addOutput(Port::create<GateOutPort>(Vec(93, 170), Port::OUTPUT, module, MentalGateMaker::FINISH_OUT)); 
 
}

Model *modelMentalGateMaker = Model::create<MentalGateMaker, MentalGateMakerWidget>("mental", "MentalGateMaker", "Gate Maker", UTILITY_TAG);