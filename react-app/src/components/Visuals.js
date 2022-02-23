import React from "react";
import Histogram from "./Histogram";
import PreferenceSpace from "./PreferenceSpace";

function Visuals() {
  return (
    <div>
      <h2 style={{'background':'gainsboro','border-radius':'5px','padding':'5px'}}>Visuals</h2>
      <div className="row justify-content-center">
        <div className="col-6" style={{ minWidth: "500px" }}>
          <Histogram />
        </div>
        <div className="col-6" style={{ minWidth: "500px" }}>
          <PreferenceSpace />
        </div>
      </div>
    </div>
  );
}

export default Visuals;
