~speed=(1.0/24.0);
f={
    var midi_n, midi_hw_n, mo, mo_hw, time_now;
    MIDIClient.init;
    midi_n=MIDIClient.destinations.detectIndex({|x,i| "FLUID.*".matchRegexp(x.device)});
//    midi_n=MIDIClient.destinations.detectIndex({|x,i| "TiM.*".matchRegexp(x.device)});
    midi_hw_n=MIDIClient.destinations.detectIndex({|x,i| "FastTrack.*".matchRegexp(x.device)});
    midi_n.postln;
    midi_hw_n.postln;
    //mo=MIDIOut(0);
    //mo_hw=MIDIOut(0);
    mo_hw=MIDIOut(midi_hw_n,0,);//MIDIClient.destinations[midi_hw_n].uid);
    mo=MIDIOut(midi_n,MIDIClient.destinations[midi_n].uid);
    mo_hw.latency=0;
    mo.latency=0;
    //mo.connect(midi_n);
    //mo_hw.connect(midi_hw_n);
    mo_hw.connect(midi_hw_n);
    time_now = TempoClock.default.beats;
    TempoClock.default.schedAbs(time_now+1,{
        //mo.noteOff(16,67,0);
        //mo.noteOn(16,67,100);
        mo_hw.midiClock;
        (1.0/24.0);
    });
    TempoClock.default.schedAbs(time_now+1,{
        mo.noteOff(16,60,0);
        mo.noteOn(16,60,100);
        1.0;
    });
    //fork {
    //    var n=0;
    //    loop {
    //        mo_hw.midiClock;
    //        if (n == 0,{
    //            mo.noteOff(16,60,0);
    //            mo.noteOn(16,60,100);
    //        });
    //        n = (n + 1) % 24;
    //        (~speed*24.0).sleep;
    //    }
    //};
    //fork {
    //    loop {
    //        mo_hw.midiClock;
    //        ~speed.sleep;
    //    }
    //};
};
f.();
    
