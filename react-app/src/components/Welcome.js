import React from "react";
import {
  setActiveComponent,
  setCandidates,
  toggleMask,
  changeMode,
  setDataset
} from "../actions";
import { connect } from "react-redux";
import { selectCandidates, parsePoints, getRanges } from "../utils";

// the welcome scene containing a brief introduction and a table to obtain the user's input
class Welcome extends React.Component {

  upload = () => {
    var target = document.createElement('input');
    target.type = 'file';
    target.accept = ".csv,text/plain"
    target.click()
    target.onchange = (e) => {
      console.log(e.path[0].files)
      var reader = new FileReader();
      reader.readAsText(e.path[0].files[0]);
      reader.onload = () => {
        const text = reader.result
        // n k
        // attributes.1 ... attributes.k
        // 0.4534 ... 0.345(k)
        var points_attr = text.trim().split("\n").slice(1)
        var points = points_attr.slice(1) // the first line is <numOfPoints> <dimension>
          .map(line =>
            line
              .trim()
              .split(/\s+/)
              .map(str => parseFloat(str))
          );
        var ranges = getRanges(points)
        var attributes = points_attr[0].trim().split(/\s+/).map((item, index) => {
          let rangesItem = ranges[index]
          rangesItem.smallerBetter = false
          return [item, rangesItem]
        })
        console.log(points)
        console.log(attributes)
        this.props.setDataset(points, '', attributes)
      };
    }
    console.log(target)
  }

  handleStart = () => {
    const ranges = [];
    const mask = [];
    for (let i = 0; i < this.props.attributes.length; ++i) {
      const [attr, config] = this.props.attributes[i];
      const range = [config.low, config.high];
      if (this.props.mask[attr]) {
        for (let j = 0; j < 2; ++j) {
          const str = this.inputs[attr][j].current.value.trim();
          if (str === "") continue;
          else if (isNaN(str)) {
            alert(`${str} in range of ${attr} is not an integer`);
            return;
          } else {
            range[j] = parseFloat(str);
          }
        }
      }
      ranges.push(range);
      mask.push(this.props.mask[attr]);
    }
    let maxPoints;
    const str = this.inputs.maxPoints.current.value.trim();
    if (str === "") maxPoints = 1000;
    else if (/\d+/.test(str)) maxPoints = parseInt(str);
    else {
      alert(`${str} for Max No. of Cars is not an integer`);
      return;
    }
    const candidates = selectCandidates(
      this.props.points,
      ranges,
      mask,
      maxPoints
    );
    if (candidates.length === 0) {
      alert("No matching cars, try larger ranges");
      return;
    }
    this.props.startAlgorithm(candidates);
  };

  handleModeChange = event => {
    this.props.changeMode(event.target.value);
  };

  render() {
    this.inputs = {};
    this.props.attributes.forEach(([attr, config]) => {
      this.inputs[attr] = [React.createRef(), React.createRef()];
    });
    this.inputs.maxPoints = React.createRef();
    console.log(this.props.attributes)
    const trs = this.props.attributes.map(([attr, config]) => {
      console.log(config)

      const disabled = this.props.mask[attr] === 0;
      const { low, high } = config;
      console.log(config)
      return (
        <tr key={attr}>
          <td className="align-middle">{attr}</td>
          <td>
            <input
              type="text"
              className="form-control"
              placeholder={low}
              ref={this.inputs[attr][0]}
              disabled={disabled}
            />
          </td>
          <td>
            <input
              type="text"
              className="form-control"
              placeholder={high}
              ref={this.inputs[attr][1]}
              disabled={disabled}
            />
          </td>
          <td className="align-middle hidden">
            <input
              type="checkbox"
              checked={disabled}
              onChange={() =>
                this.props.toggleMask(attr, 1 - this.props.mask[attr])
              }
            />
          </td>
        </tr>
      );
    });
    trs.push(
      <tr key="other">
        <td className="align-middle font-weight-bold">Max No. of Cars</td>
        <td>
          <input
            type="text"
            className="form-control"
            placeholder="1000"
            ref={this.inputs.maxPoints}
          />
        </td>
        <td>
          <div className="col form-inline align-items-center">
            <label className="mr-4 col-form-label font-weight-bold">Mode</label>
            <div className="mr-3 form-check form-check-inline">
              <input
                className="form-check-input"
                type="radio"
                name="inlineRadioOptions"
                id="simplex"
                value="simplex"
                checked={this.props.mode === "simplex"}
                onChange={this.handleModeChange}
              />
              <label className="form-check-label" htmlFor="simplex">
                Simplex
              </label>
            </div>
            <div className="form-check form-check-inline">
              <input
                className="form-check-input"
                type="radio"
                name="inlineRadioOptions"
                id="random"
                value="random"
                checked={this.props.mode === "random"}
                onChange={this.handleModeChange}
              />
              <label className="form-check-label" htmlFor="random">
                Random
              </label>
            </div>
          </div>
        </td>
        <td />
      </tr>
    );
    return (
      <div className="text-center m-auto" style={{ maxWidth: "50rem" }}>
        <div type="button"
          className="btn btn-primary"
          style={{ position: 'absolute', top: '15px', right: '15px', width: "8rem" }}
          onClick={this.upload}
        >
          Upload
        </div>
        <h1>Find Your Favorite!</h1>
        <p className="lead text-muted">
          This is a demostration of an interactive system for finding your
          favorite car in a used car database. <br />
          Enter your acceptable range for each attribute (leave blank to use the
          default). <br />
          You will be presented two cars each time and you need to choose the
          one you favor more. <br />
          Click the "Start" button to find your favorite car in the database!{" "}
          <br />
        </p>
        <table className="table">
          <thead>
            <tr>
              <th className="align-middle">Attribute</th>
              <th>Lower bound</th>
              <th>Upper bound</th>
              <th className="hidden">Discard</th>
            </tr>
          </thead>
          <tbody>{trs}</tbody>
        </table>
        <div>
          <button
            type="button"
            className="btn btn-primary"
            style={{ width: "8rem" }}
            onClick={this.handleStart}
          >
            Start
          </button>
        </div>
      </div>
    );
  }
}

const mapStateToProps = ({ attributes, mask, points, mode }) => ({
  attributes,
  mask,
  points,
  mode
});

const mapDispatchToProps = dispatch => ({
  startAlgorithm: candidates => {
    dispatch(setCandidates(candidates));
    dispatch(setActiveComponent("Interaction"));
  },
  toggleMask: (attr, mask) => dispatch(toggleMask(attr, mask)),
  changeMode: mode => dispatch(changeMode(mode)),
  setDataset: (points, labels, attributes) => dispatch(setDataset(points, labels, attributes))
});

export default connect(
  mapStateToProps,
  mapDispatchToProps
)(Welcome);
