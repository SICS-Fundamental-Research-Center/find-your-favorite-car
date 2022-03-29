import React from "react";
import Question from "./Question";
import Visuals from "./Visuals";
import Stats from "./Stats";
import { connect } from "react-redux";
import imgURL from '../assets/imgs/logo-black.png';
function Interaction({ mode }) {
  if (['Random', 'Simplex', 'Parti'].includes(mode)) {
    return (
      <div style={{ display: "flex", flexDirection: 'row', justifyContent: "space-evenly"}}>
        <div>
        <Question />
        <Visuals />
        </div>
        <div style={{ margin: '20px'}}></div>
        <Stats />
      </div>
    );
  } else {
    return (
      <div>
        <img alt='' onClick={() => window.location.reload()} src={imgURL} style={{ 'width': '50px', 'position': 'absolute', 'top': '15px', 'left': '15px', 'cursor': 'pointer' }} />
        <Question />
      </div>
    )
  }
}
const mapStateToProps = ({ mode }) => ({
  mode
});
export default connect(mapStateToProps)(Interaction);
