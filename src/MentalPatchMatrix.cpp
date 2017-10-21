///////////////////////////////////////////////////////////////////
//
//  Patch Matrix Module for VCV
//
//  Strum 2017
//  strum@softhome.net
//
///////////////////////////////////////////////////////////////////

#include "mental.hpp"
#include "dsp/digital.hpp"

////// Constructor
struct MentalPatchMatrix : Module {
	enum ParamIds {
    SWITCHES,
		NUM_PARAMS = SWITCHES + 100
	};  
	enum InputIds {		
		INPUTS,
    NUM_INPUTS = INPUTS + 10
	};
	enum OutputIds {
		OUTPUTS,    
		NUM_OUTPUTS = OUTPUTS + 10
	};

  SchmittTrigger switch_triggers[10][10];
  bool switch_states[10][10] = 
  {{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}};
  
  float switch_lights[10][10] = 
  {{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}};
    
  
  float input_values[10] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  float sums[10] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}; 
  
	MentalPatchMatrix() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {}
	void step();
  
  json_t *toJson()
  {
		json_t *rootJ = json_object();
    
    // button states
		json_t *button_statesJ = json_array();
		for (int i = 0; i < 10; i++)
    {
			for (int j = 0; j < 10; j++)
      {
        json_t *button_stateJ = json_integer((int) switch_states[i][j]);
			  json_array_append_new(button_statesJ, button_stateJ);
      }
		}
		json_object_set_new(rootJ, "buttons", button_statesJ);
    
    return rootJ;
  }
  
  void fromJson(json_t *rootJ)
  {
    // button states
		json_t *button_statesJ = json_object_get(rootJ, "buttons");
		if (button_statesJ)
    {
			for (int i = 0; i < 10; i++)
      {
        for (int j = 0; j < 10; j++)
        {
				  json_t *button_stateJ = json_array_get(button_statesJ, i*10 + j);
				  if (button_stateJ)
					  switch_states[i][j] = !!json_integer_value(button_stateJ);
        }
      }
		}  
  }  
};

////// Step function
void MentalPatchMatrix::step() {
	
  for ( int i = 0 ; i < 10 ; i++)
  {
   sums[i] = 0.0;
  }
  // deal with buttons
  for (int i = 0 ; i < 10 ; i++)
  {
   for (int j = 0 ; j < 10 ; j++)
   {
     if (switch_triggers[i][j].process(params[SWITCHES+j*10 + i].value))
     {
		   switch_states[i][j] = !switch_states[i][j];
	   }
	   switch_lights[i][j] = switch_states[i][j] ? 1.0 : 0.0;
   }
  }
  // get inputs
  for (int i = 0 ; i < 10 ; i++)
  {
    input_values[i] = inputs[INPUTS + i].value;
  }
  
  // add inputs 
  
  for (int i = 0 ; i < 10 ; i++)
  {
   for (int j = 0 ; j < 10 ; j++)
   {
     if (switch_states[j][i]) sums[i] += input_values[j];
   }
  }
  /// outputs  
  for (int i = 0 ; i < 10 ; i++)
  {
    outputs[OUTPUTS + i].value = sums[i];    
  }  
}

///// Gui
MentalPatchMatrixWidget::MentalPatchMatrixWidget() {
	MentalPatchMatrix *module = new MentalPatchMatrix();
	setModule(module);
	box.size = Vec(15*19, 380);
  
  int top_row = 75;
  int row_spacing = 25; 
  int column_spacing = 25;
	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		//panel->setBackground(SVG::load("plugins/mental/res/MentalPatchMatrix.svg"));
    panel->setBackground(SVG::load(assetPlugin(plugin,"res/MentalPatchMatrix.svg")));
		addChild(panel);
	}
	for (int i = 0 ; i < 10 ; i++)
  {
	 addInput(createInput<PJ301MPort>(Vec(3, i * row_spacing + top_row), module, MentalPatchMatrix::INPUTS + i));  
   addOutput(createOutput<PJ301MPort>(Vec(33 + i * column_spacing , top_row + 10 * row_spacing), module, MentalPatchMatrix::OUTPUTS + i));
   for(int j = 0 ; j < 10 ; j++ )
   {
     addParam(createParam<LEDButton>(Vec(35 + column_spacing * j, top_row + row_spacing * i), module, MentalPatchMatrix::SWITCHES + i + j * 10, 0.0, 1.0, 0.0));
     addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(35 + column_spacing * j + 5, top_row + row_spacing * i + 5), &module->switch_lights[i][j]));
   }
	}
  
}
