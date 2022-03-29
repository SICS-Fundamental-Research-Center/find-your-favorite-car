import React from "react";
import { connect } from "react-redux";
import { getMaxDist } from "../utils";
import ConvexHull from "./ConvexHull";
import HistogramPrefer from "./HistogramPrefer";
import Legend from "./Legend";
import PointsHull from "./PointsHull";
import {
  setLeftPrefer
} from "../actions";
// to visualize the preference space.
class PreferenceSpace extends React.Component {
  constructor(props) {
    super(props);
    const leftPoints = this.props.leftPoints.map(idx => {
      return this.props.candidates[idx].map((x, idx) => (
        (x - this.props.attributes[idx][1].low) / (this.props.attributes[idx][1].high - this.props.attributes[idx][1].low)
      ))
    })
    this.state = {
      index: 0,
      convexHullVertices: props.vertices.map(vert => vert.slice(0, 3)),
      pointsHullVertices: leftPoints.map(vert => vert.slice(0, 3))
    };
  }

  updateContextHullVertices = index => {
    const convexHullVertices = this.props.vertices.map(vertext => {
      const vert = [];
      for (let i = 0; i < 3; ++i) {
        vert.push(vertext[(i + index) % 4]);
      }
      return vert;
    });
    this.setState({ convexHullVertices });
  };

  updatePointsHullVertices = index => {
    const leftPoints = this.props.leftPoints.map(idx => {
      return this.props.candidates[idx].map((x, idx) => (
        (x - this.props.attributes[idx][1].low) / (this.props.attributes[idx][1].high - this.props.attributes[idx][1].low)
      ))
    })
    const pointsHullVertices = leftPoints.map(vertext => {
      const vert = [];
      for (let i = 0; i < 3; ++i) {
        vert.push(vertext[(i + index) % 4]);
      }
      return vert;
    });
    this.setState({ pointsHullVertices });
  }

  componentDidUpdate(prevProps) {
    if (this.props.vertices !== prevProps.vertices) {
      this.updateContextHullVertices(this.state.index);
    }
    if (this.props.leftPoints !== prevProps.leftPoints) {
      this.updatePointsHullVertices(this.state.index);
    }
    // console.log('set maxDist');
    // const maxDist = getMaxDist(this.props.vertices);
    // this.props.setLeftPrefer(maxDist);
  }

  changeStartingIndex = index => {
    this.setState({ index });
    // explicitly pass index to this.updateContextHullVertices
    // because this.state.index is not updated immediately,
    // i.e., this.state.index has still the old value at this line.
    this.updateContextHullVertices(index);
    this.updatePointsHullVertices(index);
  };

  render() {
    console.log('convertexs', this.props.vertices);
    // console.log('verticesHull', this.state.convexHullVertices);
    // console.log('leftverticeHull', this.state.pointsHullVertices);
    return (
      <div classname="row justify-content-center">
        <h4>Preference Space and Data Space</h4>

        <div style={{ display: "flex" }}>
          <ConvexHull vertices={this.state.convexHullVertices} />

          <div style={{ top: "2rem", left: "20rem" }}>
            <Legend changeStartingIndex={this.changeStartingIndex} />
          </div>
          <PointsHull vertices={this.state.pointsHullVertices} />
        </div>
      </div>
    );
  }
}

const mapStateToProps = ({
  vertices,
  leftPoints,
  attributes,
  candidates
}) => ({
  vertices,
  leftPoints,
  attributes,
  candidates
});

const mapDispatchToProps = dispatch => ({
  setLeftPrefer: maxDist => {
    dispatch(setLeftPrefer(maxDist));
  },
})

export default connect(mapStateToProps, mapDispatchToProps)(PreferenceSpace);
