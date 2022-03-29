import React from "react";
import Histogram from "./Histogram";
import HistogramPrefer from "./HistogramPrefer";
import PreferenceSpace from "./PreferenceSpace";

function Visuals() {
  return (
    <div>
      <h2 style={{ 'background': 'gainsboro', 'borderRadius': '5px', 'padding': '5px' }}>Visuals</h2>
      <div className="row justify-content-center">
        <div className=""
          style={{ display: 'flex', justifyContent: 'sapce-evenly' }}>
          <Histogram />
          <div style={{ width: '100px' }}></div>
          <HistogramPrefer />
        </div>
        <div className="">
          <PreferenceSpace />
        </div>
      </div>
    </div>
  );
}

export default Visuals;
