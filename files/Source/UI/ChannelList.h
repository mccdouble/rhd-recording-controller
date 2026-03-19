/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2014 Open Ephys

    ------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef __CHANNELLIST_H_2AD3C591__
#define __CHANNELLIST_H_2AD3C591__

#include <VisualizerEditorHeaders.h>

namespace RhythmNode
{

class DeviceThread;
class DeviceEditor;
class ChannelComponent;

class ChannelList : public Component,
                    public Button::Listener,
                    public ComboBox::Listener
{
public:
    /** Constructor */
    ChannelList (DeviceThread* board,
                 DeviceEditor* editor);

    /** Destructor */
    ~ChannelList() {}

    /** Updates label colors */
    void lookAndFeelChanged() override;

    /** Disables all channels */
    void disableAll();

    /** Enables all channels */
    void enableAll();

    /** Button callback */
    void buttonClicked (Button* btn) override;

    /** ComboBox callback */
    void comboBoxChanged (ComboBox* b) override;

    /** Updates layout of channel list */
    void update();

    /** Returns the maximum number of channels (used for setting layout) */
    int getMaxChannels() { return maxChannels; }

private:
    Array<float> gains;
    Array<ChannelInfoObject::Type> types;

    bool chainUpdate;

    DeviceThread* board;
    DeviceEditor* editor;

    //Acute Config only
    std::unique_ptr<UtilityButton> impedanceButton; //measures impedance.
    std::unique_ptr<UtilityButton> saveImpedanceButton; //saves impeadnce to file.

    //G3 Chip Stim Specific
    //galvani 3 waveform parameters
    int amplitude, period, pulse_width, edge_sel, pulse_count;

    //g3 pattern stim parameters
    int repeatCount;

    //selected output type
    //false (0) is pulse output
    //true  (1) is pattern output
    int selectedStimOutputType;
    
    //output type select
    std::unique_ptr<ComboBox> outputTypeSelect;
    std::unique_ptr<Label> outputTypeLabel;

    //upload stim param buttons
    std::unique_ptr<ColourButton> uploadStimParamButton;
    
    //pattern stim params
    std::unique_ptr<ColourButton> selectPatternFileButton;
    std::unique_ptr<TextEditor> patternRepeatCount;
    std::unique_ptr<Label> repeatLabel;

    //pulse stim params
    std::unique_ptr<Label> waveformInputLabel;
    std::unique_ptr<TextEditor> amplitudeInput;
    std::unique_ptr<TextEditor> periodInput;
    std::unique_ptr<TextEditor> pulseWidthInput;
    std::unique_ptr<ComboBox> edgeSelectBox;
    std::unique_ptr<TextEditor> pulseCountInput;

    int enabledLEDS[64];

    //galvani 3 LED Channel Toggle Buttons, Depends on the probe
    //geometry UPDATE THIS 
    std::unique_ptr<ToggleButton> toggleButtons[64];
    int shankCount = 4;
    int buttonCountPerShank = 16;
    int totalButtons = shankCount * buttonCountPerShank;

    //displays if pattern output is selected. disables toggle button LEDS.
    std::unique_ptr<Label> patternOutputLabel;

    //**G3 LED Channel Mapping array
    int ledMapping[64] = { 16, 31, 17, 30, 18, 29, 19, 28, 20, 27, 21, 26, 22, 25, 23, 24, 0, 15, 1,
        14, 2, 13, 3, 12, 4, 11, 5, 10, 6, 9, 7, 8, 47, 32, 46, 33, 45, 34, 44, 35, 43, 36, 42, 37,
        41, 38, 40, 39, 63, 48, 62, 49, 61, 50, 60, 51, 59, 52, 58, 53, 57, 54, 56, 55 };


    OwnedArray<Label> staticLabels;
    OwnedArray<ChannelComponent> channelComponents;

    int maxChannels;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChannelList);
};

} // namespace RhythmNode
#endif // __RHD2000EDITOR_H_2AD3C591__
