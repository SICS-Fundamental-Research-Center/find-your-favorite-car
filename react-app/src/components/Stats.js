import React from "react";
import { connect } from "react-redux";
import classNames from "classnames";
import PointsHull from "./PointsHull";

// statistics including the number of cars pruned/left.
class Stats extends React.Component {
  constructor(props) {
    super(props);
    this.attributes = [];
    this.props.attributes.forEach(([attr, config]) => {
      if (this.props.mask[attr]) {
        this.attributes.push(attr);
      }
    });
  }

  render() {
    let ths = [<th key="Step No.">Step</th>];
    this.attributes.forEach(attr => {
      ths.push(<th key={attr}>{attr}</th>);
    });
    const prunedTrs = this.props.prunedPoints.map(([idx, step], i) => (
      <tr key={i} data-toggle="tooltip" title={this.props.labels[idx]}>
        {[<td>{step}</td>].concat(this.props.candidates[idx].map((x, j) => {
          if (this.props.mask[this.props.attributes[j][0]]) {
            return <td key={j}>{x}</td>
          }
        }
        ))}
      </tr>
    ));
    const leftTrs = this.props.leftPoints.map((idx, i) => (
      <tr key={i} data-toggle="tooltip" title={this.props.labels[idx]}>
        {this.props.candidates[idx].map((x, j) => {
          if (this.props.mask[this.props.attributes[j][0]]) {
            return <td key={j}>{x}</td>
          }
        })}
      </tr>
    ));
    const leftPoints = this.props.leftPoints.map(idx => {
      return this.props.candidates[idx].map((x, idx) => (
        (x - this.props.attributes[idx][1].low) / (this.props.attributes[idx][1].high - this.props.attributes[idx][1].low)
      ))
    })
    // console.log('pointsleft', leftPoints, this.props.attributes);
    return (
      <div>
        <h2 style={{ 'background': 'gainsboro', 'borderRadius': '5px', 'padding': '5px' }}>Statistics</h2>
        <div className="" style={{ display: "flex", flexDirection: "column", justifyContent: "space-between" }}>
          <div className="">
            <h4>The No. of Tuples Pruned: {this.props.prunedPoints.length}</h4>
            <table
              className={classNames("table", "table-hover", {
                "table-fixed": prunedTrs.length > 7
              })}
              style={{
                minWidth: "37rem"
              }}
            >
              <thead>
                <tr>{ths}</tr>
              </thead>
              <tbody>{prunedTrs}</tbody>
            </table>
          </div>
          <div style={{ background: 'gainsboro', height: '20px', margin: '10px' }}></div>
          <div className="">
            <h4>
              The No. of Tuples Left in the Database:{" "}
              {this.props.leftPoints.length}
            </h4>
            <table
              className={classNames("table", "table-hover", {
                "table-fixed": leftTrs.length > 7
              })}
              style={{
                minWidth: "30rem"
              }}
            >
              <thead>
                <tr>{ths.slice(1)}</tr>
              </thead>
              <tbody>{leftTrs}</tbody>
            </table>
          </div>
        </div>
        {/* <div className="row"> */}
        {/* <PointsHull vertices={leftPoints.map(vert => vert.slice(0, 3))} /> */}
        {/* </div> */}
      </div>
    );
  }
}

const mapStateToPropsStats = ({
  labels,
  candidates,
  prunedPoints,
  leftPoints,
  attributes,
  mask
}) => ({
  labels,
  candidates,
  prunedPoints,
  leftPoints,
  attributes,
  mask
});

export default connect(mapStateToPropsStats)(Stats);
