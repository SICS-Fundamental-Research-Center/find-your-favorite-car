import React from "react";
import { connect } from "react-redux";
import {
  setActiveComponent,
  setLeftPoints,
  prunePoints,
  updateConvexHull
} from "../actions";
import {
  array2Vector2D,
  vector2Array,
  getPrunedIndices,
  vector2Array2D
} from "../utils";

const SIMPLEX = 2;
const RANDOM = 1;

// iteratively ask the user to choose the preferred car from a pair of cars.
class Question extends React.Component {
  constructor(props) {
    super(props);
    const smallerBetter = new window.Module.VectorInt();
    this.attributes = [];
    console.log(this.props.attributes)
    this.props.attributes.forEach(([attr, config]) => {
      if (this.props.mask[attr]) {
        smallerBetter.push_back(config.smallerBetter ? 1 : 0);
        this.attributes.push(attr);
      }
    });
    console.log(this.props.candidates)
    const candidatesVec = array2Vector2D(this.props.candidates);
    console.log(candidatesVec)

    this.runner = new window.Module.AlgorithmRunner(
      candidatesVec,
      smallerBetter,
      this.props.mode === "random" ? RANDOM : SIMPLEX
    );
    candidatesVec.delete();
    this.prevIndices = this.runner.getCandidatesIndices();
    this.props.setLeftPoints(vector2Array(this.prevIndices));
    if (this.prevIndices.size() < 2) {
      this.state = { pair: [] };
      this.stopInteraction();
    } else {
      const vec = window.Module.readConvexHullVertices();
      const convexHullVertices = vector2Array2D(vec);
      vec.delete();
      this.props.updateConvexHull(convexHullVertices);
      const indices = this.runner.nextPair();
      this.state = {
        pair: vector2Array(indices)
      };
    }
  }

  choose = idx => {
    const option = idx + 1;
    this.runner.choose(option);
    const convexHullVertices = vector2Array2D(
      window.Module.readConvexHullVertices()
    );
    this.props.updateConvexHull(convexHullVertices);
    const currIndices = this.runner.getCandidatesIndices();
    const prunedIndices = getPrunedIndices(this.prevIndices, currIndices);
    const questioNo = this.props.numLeftPoints.length;
    this.props.prunePoints(prunedIndices, questioNo);
    this.prevIndices.delete();
    this.prevIndices = currIndices;
    if (currIndices.size() < 2) {
      this.stopInteraction();
    } else {
      const indices = this.runner.nextPair();
      this.setState({
        pair: vector2Array(indices)
      });
    }
  };

  stopInteraction = () => {
    this.prevIndices.delete();
    this.runner.delete();
    this.props.showResult();
  };

  render() {
    const ths = [<th key="Option No.">Option</th>];
    this.attributes.forEach(attr => {
      ths.push(<th key={attr}>{attr}</th>);
    });
    ths.push(<th key="chooseButton" />);

    const trs = this.state.pair.map((idx, i) => {
      const tds = [<td key="Option No.">{i + 1}</td>];
      this.props.candidates[idx].forEach((x, j) => {
        tds.push(<td key={j}>{x}</td>);
      });
      tds.push(
        <td key="chooseButton">
          <button
            type="button"
            className="btn btn-outline-success btn-sm"
            onClick={() => this.choose(i)}
          >
            Choose
          </button>
        </td>
      );
      return (
        <tr key={i} data-toggle="tooltip" title={this.props.labels[idx]}>
          {tds}
        </tr>
      );
    });
    return (
      <div>
        <h2 style={{'background':'gainsboro','border-radius':'5px','padding':'5px'}}>Your Choice</h2>
        <h4>
          Q{this.props.numLeftPoints.length}: Choose the Car You Favor More
          among the Following Options
        </h4>
        <div className="row justify-content-center align-items-center">
          <div className="col-md-8">
            <table className="table table-hover text-center">
              <thead>
                <tr>{ths}</tr>
              </thead>
              <tbody>{trs}</tbody>
            </table>
          </div>
          <div className="col-md-1">
            <button
              type="button"
              className="btn btn-primary"
              onClick={this.stopInteraction}
            >
              Stop
            </button>
          </div>
        </div>
      </div>
    );
  }
}

const mapStateToProps = ({
  labels,
  candidates,
  mask,
  attributes,
  numLeftPoints,
  mode
}) => ({
  labels,
  candidates,
  mask,
  attributes,
  numLeftPoints,
  mode
});

const mapDispatchToProps = dispatch => ({
  showResult: () => {
    dispatch(setActiveComponent("Result"));
  },
  setLeftPoints: indices => {
    dispatch(setLeftPoints(indices));
  },
  prunePoints: (indices, step) => {
    dispatch(prunePoints(indices, step));
  },
  updateConvexHull: vertices => {
    dispatch(updateConvexHull(vertices));
  }
});

export default connect(
  mapStateToProps,
  mapDispatchToProps
)(Question);
