import React from "react";
import Welcome from "./Welcome";
import Interaction from "./Interaction";
import Result from "./Result";
import { connect } from "react-redux";
// import imgURL from '../assets/imgs/logo-black.png';
// import imgFavorite from '../assets/imgs/favorite.png';
// import { getRanges } from "../utils";


function App({ activeComponent }) {
  // var upload = () => {
  //   var target = document.createElement('input');
  //   target.type = 'file';
  //   target.accept = ".csv,text/plain"
  //   target.click()
  //   target.onchange = (e) => {
  //     console.log(e.path[0].files)
  //     var reader = new FileReader();
  //     reader.readAsText(e.path[0].files[0]);
  //     reader.onload = () => {
  //       const text = reader.result
  //       // n k
  //       // attributes.1 ... attributes.k
  //       // 0.4534 ... 0.345(k)
  //       var points_attr = text.trim().split("\n").slice(1)
  //       // the first line is <numOfPoints> <dimension>
  //       var points = points_attr.slice(1).map(line => line.trim().split(/\s+/).map(str => parseFloat(str)));
  //       var ranges = getRanges(points)
  //       var attributes = points_attr[0].trim().split(/\s+/).map((item, index) => {
  //         let rangesItem = ranges[index]
  //         rangesItem.smallerBetter = false
  //         return [item, rangesItem]
  //       })
  //       console.log(attributes)
  //       this.props.setDataset(points, '', attributes)
  //     };
  //   }
  // }
  switch (activeComponent) {
  //   case "Home":
  //     return <div>
  //       <img alt='' onClick={() => window.location.reload()} src={imgURL} style={{ 'width': '50px', 'position': 'absolute', 'top': '15px', 'left': '15px', 'cursor': 'pointer' }} />
  //       <img alt='' src={imgFavorite} style={{ 'width': '400px' }} />
  //       <div>
  //         <div type="button"
  //           className="btn btn-primary"
  //           style={{ width: "8rem" }}
  //           onClick={upload}
  //         >
  //           Upload
  //         </div>
  //         <div style={{'width': '100px','display':'inline-block'}}></div>
  //         <div type="button"
  //           className="btn btn-primary"
  //           style={{ width: "8rem" }}
  //           onClick={()=>activeComponent= 'Welcome'}

  //         >
  //           Sample
  //         </div>
  //       </div>

  //     </div>
    case "Welcome":
      return <Welcome />;
    case "Interaction":
      return <Interaction />;
    case "Result":
      return <Result />;
    default:
      return <div />;
  }
}

const mapStateToProps = ({ activeComponent }) => ({
  activeComponent
});

export default connect(mapStateToProps)(App);
