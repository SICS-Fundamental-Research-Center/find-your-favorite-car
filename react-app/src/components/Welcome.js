import React from "react";
import {
  setActiveComponent,
  setCandidates,
  toggleMask,
  changeMode,
  changeK,
  setDataset
} from "../actions";
import { connect } from "react-redux";
import { selectCandidates, getRanges } from "../utils";
import imgURL from '../assets/imgs/logo-black.png';
import imgFavorite from '../assets/imgs/favorite.png';

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
        // for points_attr, the first line is <numOfPoints> <dimension>
        var points = points_attr.slice(1).map(line => line.trim().split(/\s+/).map(str => isNaN(parseFloat(str)) ? 0 : parseFloat(str)));
        console.log(points);
        var ranges = getRanges(points)
        console.log(ranges)
        var attributes = points_attr[0].trim().split(/\s+/).map((item, index) => {
          let rangesItem = ranges[index]
          rangesItem.smallerBetter = false
          return [item, rangesItem]
        })
        console.log(attributes)
        if (points.length < 5000) {
          alert("please upload data more then 5000 rows!");
          return;
        }
        this.props.setDataset(points, '', attributes)
      };
    }
  }

  handleStart = () => {
    if (Object.values(this.props.mask).filter((i) => i === 1).length < 2) {
      return alert('You select at least two properties')
    }
    

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
    console.log('input number', str);
    if (str === "") maxPoints = 5000;
    else if (/\d+/.test(str)) maxPoints = parseInt(str);
    else if (maxPoints>50000 || maxPoints<5000) return alert('100 ≤ maximum range ≤ 50000');
    else {
      alert(`${str} for Maximum items is not an integer`);
      return;
    }

    if (maxPoints>50000 || maxPoints<5000) return alert('100 ≤ maximum range ≤ 50000');

    if (!['Random', 'Simplex', 'Parti'].includes(this.props.mode)) {
      var K = parseInt(prompt('Please input an integer K >= ' + this.props.attributes.length))
      if (!(/(^[1-9]\d*$)/.test(K) && K >= this.props.attributes.length)) return alert('Illegal number!')
      this.props.changeK(K)
    }
    const candidates = selectCandidates(
      this.props.points,
      ranges,
      mask,
      maxPoints
    );
    if (candidates.length === 0) {
      alert("No matching Tuples, try larger ranges");
      return;
    }
    this.props.startAlgorithm(candidates);
  };

  handleChange = event => {
    if (['graphDP', 'biSearch', 'sweepDP'].includes(event.target.value)) {
      console.log(this.props.mask)
      if (Object.values(this.props.mask).filter((i) => i === 1).length > 2) {
        alert(event.target.value + ' only supports 2D data!')
        event.target.value = this.props.mode
        return
      }
    }
    console.log(event.target.value)
    this.props.changeMode(event.target.value);
  };

  render() {
    // Init inputs
    this.inputs = {};
    this.props.attributes.forEach(([attr, config]) => {
      this.inputs[attr] = [React.createRef(), React.createRef()];
    });
    this.inputs.maxPoints = React.createRef();
    // Iterate attributes
    const trs = this.props.attributes.map(([attr, config], index) => {
      const disabled = this.props.mask[attr] === 0;
      const { low, high } = config;
      return (
        <tr key={attr}>
          <td
            className="align-middle">
            <span
              onClick={() => this.props.toggleMask(attr, 1 - this.props.mask[attr])}
              className={`${this.props.mask[attr] ? "attribute-select" : null}`}
              style={{ 'cursor': 'pointer', 'textDecoration': this.props.mask[attr] ? '' : 'line-through' }}
            >{attr}</span>
          </td>
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
          <td className="align-middle" onClick={() => { this.props.attributes[index][1].smallerBetter = !this.props.attributes[index][1].smallerBetter; this.setState({ attributes: this.props.attributes }) }}>
            {this.props.attributes[index][1].smallerBetter ? <span style={{
              'background': '#ff8737',
              'padding': '5px',
              'borderRadius': '5px',
              'color': 'white'
            }}>Yes</span> : <span>No</span>}
          </td>
        </tr>
      );
    });
    trs.push(
      <tr key="other">
        <td className="align-middle font-weight-bold">Max No. of Tuples</td>
        <td>
          <input
            type="text"
            className="form-control"
            placeholder='5000'
            ref={this.inputs.maxPoints}
          />
        </td>
        <td style={{ 'textAlign': 'left' }}>
          {/* <div className="col form-inline align-items-center"> */}
          <label className="mr-4 col-form-label font-weight-bold">Mode</label>
          <div className="mr-3 form-check form-check-inline">
            <div>
              <select onChange={this.handleChange}>
                <option value="DMM">DMM</option>
                <option value="cube">cube</option>
                <option value="sphere">sphere</option>
                <option value="Simplex">Simplex</option>
                <option value="Random">Random</option>
                <option value="Random">Parti</option>
                <option value="graphDP">graphDP</option>
                <option value="biSearch">biSearch</option>
                <option value="sweepDP">sweepDP</option>
                <option value="IncGreedy">IncGreedy</option>
              </select>
            </div>
          </div>
        </td>
      </tr>
    );
    return (
      <div className="text-center m-auto" style={{ maxWidth: "50rem" }}>
        <img alt='' onClick={() => window.location.reload()} src={imgURL} style={{ 'width': '50px', 'position': 'absolute', 'top': '15px', 'left': '15px', 'cursor': 'pointer' }} />
        <div type="button"
          title="Format of Input file
                ------------------------------------&#13;
                The format of the first line is: n d
                n - the number of tuples in the dataset, integer
                d - dimensionality, integer
                The format of the second line is:
                <A1> <A2> ... <Ad>
                Ai - the i-th attribute name, string
                The format of the following n lines is
                <dim 1> <dim 2> ... <dim d>.
                Each line corresponds to a tuple."
          className="btn btn-primary upload"
          style={{ position: 'absolute', top: '15px', right: '15px', width: "8rem" }}
          onClick={this.upload}
        >
          Upload
        </div>
        <img alt='' src={imgFavorite} style={{ 'width': '400px' }} />
        <br/>
        <br/>
        <table className="table">
          <thead>
            <tr>
              <th className="align-middle">Attribute</th>
              <th>Lower bound</th>
              <th>Upper bound</th>
              <th>Smaller better</th>
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
  changeK: K => dispatch(changeK(K)),
  setDataset: (points, labels, attributes) => dispatch(setDataset(points, labels, attributes))
});

export default connect(
  mapStateToProps,
  mapDispatchToProps
)(Welcome);
