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
        text_boxes = List();
    //num_note_param_sets.do({
    //    |n|
    //    midi_note_commands.do({|x,m|
    //        var idx = (n*num_ccs_note + m),
    //        sliders.add(
    //            Slider.new(win,Rect(x1,y1,wi,he)));
    //    });
    //});
    //midi_global_commands.do({
    //    |x,m|
    //    var idx = (tot_num_ccs_note + m),
    //    sliders.add(
    //        Slider.new(win,Rect(x1,y1,wi,he)));
    //});
    var text_test, text_test2;
    midi_note_commands.sort.reverse.at(1).postln;
    num_note_param_sets.do({
        |n|
        midi_note_commands.do({|x,m|
            var idx = (n*num_ccs_note + m),
                y1 = (idx % row_height) * (slider_height + gui_element_gap),
                x1 = (idx / row_height).floor * (slider_width + gui_element_gap),
                he = (slider_height + gui_element_gap),
                wi = slider_width;
            sliders.add(
                Slider.new(win,Rect(x1,y1,wi,he)));
        });
    });
    midi_global_commands.do({
        |x,m|
        var idx = (tot_num_ccs_note + m),
            y1 = (idx % row_height) * (slider_height + gui_element_gap),
            x1 = (idx / row_height).floor * (slider_width + gui_element_gap),
            he = (slider_height + gui_element_gap),
            wi = slider_width;
        sliders.add(
            Slider.new(win,Rect(x1,y1,wi,he)));
    });
    win.bounds = Rect(
        0,
        0,
        ((tot_num_ccs / row_height).floor + 1) * (slider_width + gui_element_gap),
        (row_height) * (slider_height + gui_element_gap));
    text_test = StaticText(win,Rect(10,10,10,10)).string_("hello").font_(
                    Font("Monaco",10));
    text_test2= StaticText(win,Rect(10,10,20,20)).string_("hello").font_(
                    Font("Monaco",10));
    win.onClose = { 0.exit };
    win.front;
};
a.();
