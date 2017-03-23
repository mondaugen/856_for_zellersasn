a={
    // synth parameter stuff
    var num_ccs_note=18,
        num_ccs_global=10,
        num_note_param_sets=3,
        num_presets=3,
        tot_num_ccs_note=(num_note_param_sets * num_ccs_note),
        tot_num_ccs=(tot_num_ccs_note + num_ccs_global),
        midi_note_commands = [
        "PITCH_FINE1",
        "PITCH_FINE2",
        "PITCH_FINE3",
        "ENV",
        "SUS",
        "PITCH1",
        "PITCH2",
        "PITCH3",
        "GAIN",
        "POS",
        "STRIDE",
        "OFFSET",
        "FBK_RATE",
        "EVENT_DELTA",
        "NUM_REPS",
        "STRIDE_STATE",
        "INTERM",
        "SWING"
    ],
        midi_global_commands = [
        "TEMPO_COARSE",
        "TEMPO_FINE",
        "TEMPO_SCALE",
        "TEMPO_NUDGE",
        "PRESET_STORE",
        "PRESET_RECALL",
        "REC",
        "PLAY",
        "REC_MODE",
        "FBK_STATE"
    ],
        midi_note_commands_num_steps = [
        128,
        128,
        128,
        128,
        128,
        128,
        128,
        128,
        128,
        128,
        128,
        128,
        128,
        128,
        128,
        2  ,
        128,
        128,
    ],
        midi_global_commands_num_steps = [
        128,
        128,
        128,
        128,
        num_presets,
        num_presets,
        2,
        2,
        3,
        2 
    ],
        midi_commands_num_steps = List();

    // gui size stuff
    var slider_height = 15,
        slider_width = 128,
        gui_element_gap = 0,
        row_height = num_ccs_note;
    
    var win = Window.new(
        "856 Controls",
        Rect(0,0,slider_width,(slider_width + gui_element_gap)*tot_num_ccs)
    ),
        sliders = List(),
        text_boxes = List(),
        all_midi_commands = List(),
        noteon_pitch_slider,
        noteon_vel_slider,
        noteon_slider_num_steps = 127,
        noteon_button,
        noteon_pitch_text,
        noteon_vel_text,
        noteon_title = "NOTE_ON",
        noteon_button_width = slider_height,
        noteon_button_height = slider_height;
    var longest_cc_name,
        font_pixel_size = 10,
        text_width,
        text_height = font_pixel_size,
        text_font = Font("Monospace",font_pixel_size),
        test_button;
    // MIDI setup stuff
    var midi_device_name = "FastTrack",
        midi_device_idx,
        midi_out,
        midi_channel = 0,
        midi_data_range_max = 127;
    MIDIClient.init;
    midi_device_idx = MIDIClient.destinations.detectIndex({
        |x|
        x.name.contains(midi_device_name)
    });
    "Using MIDI device: ".post;
    MIDIClient.destinations.at(midi_device_idx).name.postln;
    midi_out = MIDIOut(0);
    midi_out.connect(midi_device_idx);
    if (thisProcess.argv.size >= 1,{
        midi_channel=thisProcess.argv[0].asInteger;
    });

    num_note_param_sets.do({
        |n|
        all_midi_commands.addAll(midi_note_commands);
        midi_commands_num_steps.addAll(midi_note_commands_num_steps);
    });
    all_midi_commands.addAll(midi_global_commands);
    midi_commands_num_steps.addAll(midi_global_commands_num_steps);
    longest_cc_name = all_midi_commands.asSortedList.reverse.at(1).postln;
    text_width = longest_cc_name.size * font_pixel_size;
    all_midi_commands.do({
        |x,m|
        var idx = m,
            he = (slider_height),
            wi = slider_width,
            text_y1 = (idx % row_height) * (slider_height + gui_element_gap),
            text_x1 = ((idx / row_height).floor 
                    * (slider_width + gui_element_gap + text_width)),
            y1 = text_y1,
            x1 = text_x1 + text_width,
            slider_step_size = 1.asFloat/(midi_commands_num_steps[m]-1);

        sliders.add(
            Slider.new(win,Rect(x1,y1,wi,he)).mouseUpAction_({
                |x|
                var datum = (x.value * (midi_commands_num_steps[m] - 1)).floor;
                "Sending ".post;
                "CC: ".post;
                m.post;
                " Val: ".post;
                datum.post;
                "".postln;
                midi_out.control(midi_channel,m,datum);
            }).step_(
                slider_step_size
            ).action_({
                |x|
                var datum = (x.value * (midi_commands_num_steps[m] - 1)).floor;
                "CC: ".post;
                m.post;
                " Val: ".post;
                datum.post;
                "".postln;
            })
        );
        text_boxes.add(
            StaticText(win,
                Rect(
                    text_x1,
                    text_y1,
                    text_width,
                    text_height)).string_(x).font_(text_font)
            );
    });
    noteon_pitch_slider = Slider.new(
        win,
        Rect( 
            ((tot_num_ccs / row_height).floor) 
                * (slider_width + gui_element_gap + text_width) + text_width,
            (tot_num_ccs % row_height)
                * (slider_height + gui_element_gap),
            slider_width,
            slider_height
        )
    ).step_(
        1.asFloat/(noteon_slider_num_steps - 1)
    ).action_({
        |x|
        var datum = (x.value * (noteon_slider_num_steps - 1)).floor;
        "CC: ".post;
        m.post;
        " Val: ".post;
        datum.post;
        "".postln;
    });
    noteon_vel_slider = Slider.new(
        win,
        Rect( 
            ((tot_num_ccs / row_height).floor) 
                * (slider_width + gui_element_gap + text_width) + text_width,
            ((tot_num_ccs % row_height) + 1)
                * (slider_height + gui_element_gap),
            slider_width,
            slider_height
        )
    ).step_(
        1.asFloat/(noteon_slider_num_steps - 1)
    ).action_({
        |x|
        var datum = (x.value * (noteon_slider_num_steps - 1)).floor;
        "CC: ".post;
        m.post;
        " Val: ".post;
        datum.post;
        "".postln;
    });
    noteon_pitch_text  = StaticText(
        win,
        Rect(
            ((tot_num_ccs / row_height).floor) 
            * (slider_width + gui_element_gap + text_width),
            (tot_num_ccs % row_height)
            * (slider_height + gui_element_gap),
            text_width,
            text_height 
        )
    ).string_(noteon_title + "PITCH").font_(text_font);
    noteon_vel_text  = StaticText(
        win,
        Rect(
            ((tot_num_ccs / row_height).floor) 
            * (slider_width + gui_element_gap + text_width),
            ((tot_num_ccs % row_height) + 1)
            * (slider_height + gui_element_gap),
            text_width,
            text_height 
        )
    ).string_(noteon_title + "VEL").font_(text_font);
    noteon_button = Button.new(
        win,
        Rect( 
            ((tot_num_ccs / row_height).floor) 
                * (slider_width + gui_element_gap + text_width) + text_width
                - noteon_button_width,
            (tot_num_ccs % row_height)
                * (slider_height + gui_element_gap),
            noteon_button_width,
            noteon_button_height
        ).postln
    ).states_(
        [["",Color.black,Color.white],["",Color.black,Color.white]]
    ).mouseDownAction_({
        midi_out.noteOn(
            midi_channel,
            noteon_pitch_slider.value *
                (noteon_slider_num_steps - 1),
            noteon_vel_slider.value *
                (noteon_slider_num_steps - 1))
    });
    win.bounds = Rect(
        0,
        0,
        ((tot_num_ccs / row_height).floor + 1) * (slider_width + gui_element_gap + text_width),
        (row_height) * (slider_height + gui_element_gap)).postln;
    win.onClose = { 
        midi_out.disconnect(midi_device_idx);
        0.exit;
    };

    win.front;
};
a.();
