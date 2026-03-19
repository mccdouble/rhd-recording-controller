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

#include "ChannelCanvas.h"

#include "ChannelList.h"

#include "../DeviceThread.h"

using namespace RhythmNode;

/**********************************************/

ChannelCanvas::ChannelCanvas (DeviceThread* board_,
                              DeviceEditor* editor_) : board (board_),
                                                       editor (editor_)
{
    channelViewport = std::make_unique<Viewport>();

    channelList = std::make_unique<ChannelList> (board, editor);

    channelViewport->setViewedComponent (channelList.get(), false);
    channelViewport->setScrollBarsShown (true, true);
    channelViewport->setScrollBarThickness (10);
    addAndMakeVisible (channelViewport.get());

    update();

    resized();
}

void ChannelCanvas::paint (Graphics& g)
{
     g.fillAll(Colours::grey);
    g.setColour(juce::Colours::black);

    // Define the width and gap of the shanks
    float shankThickness = 2.0f;
    float gap = shankThickness * 8.0f;

    // If probe is 1 Shank, 2 Shank, or 4 Shank configurations
    int shankCount = 4;

    // Calculate the starting position for the first shank and its dimensions
    float startX = 30.0f;
    float shankHeight = 500;
    float startY = 125;
    float shankWidth = 80.0f; // Width of the shank

    // Define electrode parameters
    int numElectrodes = 32; //number of electrodes per shank
    float electrodeSize = 5.0f; // Size of each electrode square
    float electrodeSpacing = (shankHeight / (numElectrodes / 2.0f)) - 3; // Vertical space between electrodes

    // Draw the shank graphics where we will place the buttons to enable/disable LEDs
    for (int i = 0; i < shankCount; ++i)
    {
        // Calculate new position for current shank.
        float xPosition = startX + i * (shankWidth + gap);

        // Draw each of the shanks
        juce::Path shankOutline;
        shankOutline.startNewSubPath(xPosition - 5, startY);
        shankOutline.lineTo(xPosition, startY + shankHeight - (shankWidth * 0.5f));
        shankOutline.lineTo(xPosition + shankWidth / 2.0f, startY + shankHeight);
        shankOutline.lineTo(xPosition + shankWidth, startY + shankHeight - shankWidth * 0.5f);
        shankOutline.lineTo(xPosition + shankWidth, startY);
        shankOutline.closeSubPath();
        g.strokePath(shankOutline, juce::PathStrokeType(shankThickness));

        // Draw the label above each shank
        juce::String labelText = "Shank " + juce::String(i + 1);
        g.setFont(14.0f);
        if (shankCount == 2) { gap = gap + 5; }
        g.drawText(labelText, (int)(xPosition - gap), (int)(startY - 20), 100, 20, juce::Justification::centredTop);

        // Draw the electrode squares and labels within the shank
        g.setFont(10.0f);
        g.setColour(juce::Colours::black);
        for (int j = 0; j < numElectrodes; ++j)
        {
            // Determine if the electrode is on the left or right
            bool isLeft = (j % 2 == 0);
            float electrodeX = isLeft ? (xPosition + electrodeSize + 10) : (xPosition + shankWidth - electrodeSize * 2 - 10);
            float electrodeY = startY + (j / 2) * electrodeSpacing + (isLeft ? 0 : electrodeSpacing / 2.0f);
            
            //lower all electrodes in the shank outline
            electrodeY = electrodeY + 25;
            
            //stagger E1 slightly to match actual probe layout.
            if(j == 31){electrodeX = electrodeX - 5;}

            //fill the electrode
            g.fillRect(electrodeX, electrodeY, electrodeSize, electrodeSize);
            juce::String electrodeLabel = "E" + juce::String((j + 1) + (32 * i));
            g.setColour(juce::Colours::black);
            g.drawText(electrodeLabel, isLeft ? electrodeX + electrodeSize - 10 : electrodeX - 10, electrodeY - 10, 20, 10,
                    isLeft ? juce::Justification::centredLeft : juce::Justification::centredRight);
           
        }
    }

    //write disclaimer that the electrode to LED button spacing is not exact and is just a visual aid.
    //Please refer to specific probe geometry for LED to Electrode Space
    juce::String disclaimer = "NOTE: Probe overlay is an approximate visual aid. Please refer to the ";
    g.drawText(disclaimer, 30, 640, 600, 10, juce::Justification::left);
    disclaimer = "specific probe geometry for exact LED and Electrodes spacing/location.";
    g.drawText(disclaimer, 30, 660, 600, 10, juce::Justification::left);
}

void ChannelCanvas::refresh()
{
    repaint();
}

void ChannelCanvas::refreshState()
{
    resized();
}

void ChannelCanvas::updateSettings()
{
    channelList->update();
    resized();
}

void ChannelCanvas::updateAsync()
{
    Timer::callAfterDelay (5, [this]
                           { updateSettings(); });
}

void ChannelCanvas::beginAnimation()
{
    channelList->disableAll();
}

void ChannelCanvas::endAnimation()
{
    channelList->enableAll();
}

void ChannelCanvas::resized()
{
    int scrollBarThickness = channelViewport->getScrollBarThickness();

    channelViewport->setBounds (0, 0, getWidth(), getHeight());

    channelList->setBounds (0, 0, getWidth() - scrollBarThickness, 200 + 22 * channelList->getMaxChannels());
}
