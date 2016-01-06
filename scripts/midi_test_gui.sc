a={
    // gui size stuff
    var slider_height = 12,
        slider_width = 100,
        gui_element_gap = 2,
        row_height = 17;
    
    // synth parameter stuff
    var num_ccs_note=17,
        num_ccs_global=10,
        num_note_param_sets=3,
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
    ];
    var win = Window.new(
        "856 Controls",
        Rect(0,0,slider_width,(slider_width + gui_element_gap)*tot_num_ccs)
    ),
        sliders = List(),
        text_boxes = List(),
        all_midi_commands = List();
    var longest_cc_name,
        font_pixel_size = 10,
        text_width,
        text_height = font_pixel_size,
        text_font = Font("Monospace",font_pixel_size),
        test_button;
    // MIDI setup stuff
    var midi_device_name;
    num_note_param_sets.do({
        |n|
        all_midi_commands.addAll(midi_note_commands);
    });
    all_midi_commands.addAll(midi_global_commands);
    longest_cc_name = all_midi_commands.asSortedList.reverse.at(1).postln;
    text_width = longest_cc_name.size * font_pixel_size;
    all_midi_commands.postln;
    all_midi_commands.do({
        |x,m|
        var idx = m,
            he = (slider_height + gui_element_gap),
            wi = slider_width,
            text_y1 = (idx % row_height) * (slider_height + gui_element_gap),
            text_x1 = ((idx / row_height).floor 
                    * (slider_width + gui_element_gap + text_width)),
            y1 = text_y1,
            x1 = text_x1 + text_width;
        sliders.add(
            Slider.new(win,Rect(x1,y1,wi,he)));
        text_boxes.add(
            StaticText(win,
                Rect(
                    text_x1,
                    text_y1,
                    text_width,
                    text_height)).string_(x).font_(text_font)
            );
    });
    win.bounds = Rect(
        0,
        0,
        ((tot_num_ccs / row_height).floor + 1) * (slider_width + gui_element_gap + text_width),
        (row_height) * (slider_height + gui_element_gap));
    win.onClose = { 0.exit };
    test_button = Button(win,Rect(10,10,10,10));
    win.front;
};
a.();
