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
  array2Vector,
  vector2Array,
  getPrunedIndices,
  vector2Array2D
} from "../utils";
import {
  FlexibleXYPlot,
  XAxis,
  YAxis,
  Hint,
  VerticalBarSeries,
  ChartLabel
} from "react-vis";
var smallerBetter, candidatesVec;

// iteratively ask the user to choose the preferred car from a pair of cars.
class Question extends React.Component {
  constructor(props) {
    super(props);
    smallerBetter = new window.Module.VectorInt();
    this.attributes = [];
    console.log(this.props.mask)
    this.props.attributes.forEach(([attr, config]) => {
      if (this.props.mask[attr]) {
        smallerBetter.push_back(config.smallerBetter ? 1 : 0);
        this.attributes.push(attr);
      }
    });
    console.log(this.props.candidates)
    candidatesVec = array2Vector2D(this.props.candidates);

    // new algorithm
    if (!['Random', 'Simplex', 'Parti'].includes(this.props.mode)) {
      var d1 = new Date();
      this.runner = new window.Module.SevenAlgorithmRunner(
        candidatesVec,
        smallerBetter,
        { 'DMM': 4, 'cube': 5, 'sphere': 6, 'graphDP': 7, 'biSearch': 8, 'sweepDP': 9, 'IncGreedy': 10 }[this.props.mode],
        this.props.K
      );
      var d2 = new Date();
      this.state = {
        performanceIndex: [{
          x: 0,
          y: this.runner.getMrr(),
          runtime: d2 - d1,
          mrr: this.runner.getMrr()
        }]
      }
      console.log(this.runner)
      candidatesVec.delete();
      return this.pageResult = 'NewAlgorithm'
    }

    // old algorithm
    this.runner = new window.Module.AlgorithmRunner(
      candidatesVec,
      smallerBetter,
      { 'Random': 1, 'Simplex': 2, 'Parti': 3 }[this.props.mode]
    );
    console.log(this.runner)
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
    return this.pageResult = 'OldAlgorithm'
  }

  next = () => {
    const selectedPoint = vector2Array(this.runner.getIndices())
    if (this.selectedPoint && this.selectedPoint.length === selectedPoint.length) return;
    this.selectedPoint = selectedPoint
    var d1 = new Date();
    this.runner.Next_IncGreedy()
    var d2 = new Date();
    console.log(d2 - d1);
    var _performanceIndex = this.state.performanceIndex
    _performanceIndex.push({
      x: _performanceIndex.length,
      y: this.runner.getMrr(),
      runtime: d2 - d1,
      mrr: this.runner.getMrr()
    })
    this.setState({ performanceIndex: _performanceIndex })
    // candidatesVec.delete();
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

  inputDimension = () => {
    var D = prompt('Please input ' + this.attributes.length + ' dimension')
    if (!D) return
    var jsArray = D.trim().split(' ').map((n) => parseInt(n))
    if (jsArray.includes(NaN) || jsArray.length !== this.attributes.length || jsArray.some((i) => i < 0)) return alert('Illegal number set!')
    this.runner.globalBest_localBest(array2Vector(jsArray))
    this.global_local_regret = { globalBest: this.props.candidates[this.runner.get_globalBestId()], localBest: this.props.candidates[this.runner.get_localBestId()], regretRatio: this.runner.get_regret_ratio() }
    this.setState({ global_local_regret: this.global_local_regret })
  }

  render() {
    if (this.pageResult === 'OldAlgorithm') {
      const ths = [<th key="Option No.">Option</th>];
      this.attributes.forEach(attr => {
        ths.push(<th key={attr}>{attr}</th>);
      });
      ths.push(<th key="chooseButton" />);

      const trs = this.state.pair.map((idx, i) => {
        const tds = [<td key="Option No.">{i + 1}</td>];
        this.props.candidates[idx].forEach((x, j) => {
          if (this.props.mask[this.props.attributes[j][0]]) {
            tds.push(<td key={j}>{x}</td>);
          }
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
          <h2 style={{ 'background': 'gainsboro', 'borderRadius': '5px', 'padding': '5px' }}>Your Choice</h2>
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
      )
    }

    // new algorithm
    let ths = [<th key="Option No.">Option ID</th>];
    this.attributes.forEach(attr => {
      ths.push(<th key={attr}>{attr}</th>)
    });
    const selectedPoint = vector2Array(this.runner.getIndices())
    console.log(selectedPoint)
    console.log(this.runner.getMrr())

    const trs = selectedPoint.slice(-this.props.K).map((id, i) => {
      const tds = [<td key="Option No.">{id}</td>];
      
      this.props.candidates[id].forEach((x, j) => {
          if (this.props.mask[this.props.attributes[j][0]]) {
            tds.push(<td key={j}>{x}</td>);
          }
      });
      return (
        <tr key={i} data-toggle="tooltip" title={this.props.attributes[id]}>
          {tds}
        </tr>
      );
    });

    return (
      <div>
        <br />
        <div style={{ 'borderRadius': '5px', 'padding': '5px', 'fontSize': '22px' }}>
          <span style={{ 'color': '#ff8737' }}>{this.props.mode}</span> algorithm selects k&lt;={this.props.K} interest points for you
        </div>
        <br />
        <table style={{ 'borderBottom': '1px solid gainsboro' }} className="table table-hover text-center">
          <thead>
            <tr>{ths}</tr>
          </thead>
          <tbody>{trs}</tbody>
        </table>
        <div style={{ 'display': 'flex', 'justifyContent': 'space-between', 'padding': '0 10px', }}>
          <div>
            Output size: {selectedPoint.length}
          </div>
          <div onClick={this.inputDimension} style={{ 'background': '#ff8737', 'color': 'white', 'padding': '5px', 'borderRadius': '5px' }}>
            Input D Dimension
          </div>
        </div>
        <br />

        {this.props.mode === 'IncGreedy' ? (
          <div>
            <div style={{ 'margin': 'auto', width: "30rem", 'height': '28rem' }}>
              <FlexibleXYPlot onMouseLeave={() => {
                this.setState({ hintValue: null })
              }}>
                <XAxis
                  tickValues={this.state.performanceIndex.map((val, idx) => idx)}
                  tickFormat={value => value.toString()}
                  style={{
                    ticks: { fill: "black" }
                  }}
                />
                <YAxis
                  style={{
                    ticks: { fill: "black" }
                  }}
                />
                <ChartLabel
                  text="Rounds"
                  includeMargin={false}
                  xPercent={0.82}
                  yPercent={1.06}
                  style={{
                    style: { fill: "black" }
                  }}
                />
                <ChartLabel
                  text="MRR(maximun regret ratio)"
                  includeMargin={false}
                  xPercent={-0.01}
                  yPercent={0.05}
                  style={{
                    transform: "rotate(-90)",
                    textAnchor: "end",
                    style: { fill: "black" }
                  }}
                />
                <VerticalBarSeries
                  color="#ff8737"
                  onNearestX={(value, { index }) => {
                    this.setState({ hintValue: '' + index })
                  }}
                  data={this.state.performanceIndex}
                  animation
                />
                {this.state.hintValue && (
                  <Hint value={this.state.hintValue}>
                    <p style={{ 'color': "black", 'textAlign': 'left' }}>
                      Runtime: {this.state.performanceIndex[this.state.hintValue].runtime}ms<br />
                      Mrr: {this.state.performanceIndex[this.state.hintValue].mrr.toFixed(4)}
                    </p>
                  </Hint>
                )}
              </FlexibleXYPlot>
            </div>
            <div style={{ 'background': '#ff8737', 'margin': 'auto', 'width': '100px', 'color': 'white', 'borderRadius': '3.25rem', 'padding': '5px' }} onClick={this.next}>Next</div>

          </div>

        ) : (
          <div style={{ 'textAlign': 'left', 'padding': '8px', 'borderRadius': '5px', 'background': 'gainsboro' }}>
            {this.props.mode} runtime: {this.state.performanceIndex[0].runtime} ms  <br /> MRR(maximun regret ratio): {this.state.performanceIndex[0].mrr}
          </div>
        )}
        <br />
        {this.global_local_regret ? (
          <div style={{ 'background': '#ff8737', 'color': 'white', 'textAlign': 'left', 'borderRadius': '5px', 'padding': '8px' }}>
            
            <div>Your global favorite tuple is:<br />
              <table style={{ 'borderBottom': '1px solid gainsboro' }} className="table table-hover text-center">
                <thead>
                  <tr>{ths}</tr>
                </thead>
                <tbody>{(() => {
                  const tds = [<td key="Option No.">{this.runner.get_globalBestId()}</td>];
                  this.global_local_regret['globalBest'].forEach((x, j) => {
                    if (this.props.mask[this.props.attributes[j][0]]) {
                      tds.push(<td key={j}>{x}</td>);
                    }
                  })
                  return tds
                })()}
                </tbody>
              </table>
            </div>

            <div>Your local favorite tuple is:<br />
              <table style={{ 'borderBottom': '1px solid gainsboro' }} className="table table-hover text-center">
                <thead>
                  <tr>{ths}</tr>
                </thead>
                <tbody>{(() => {
                  const tds = [<td key="Option No.">{this.runner.get_localBestId()}</td>];
                  this.global_local_regret['localBest'].forEach((x, j) => {
                    if (this.props.mask[this.props.attributes[j][0]]) {
                      tds.push(<td key={j}>{x}</td>);
                    }
                  })
                  return tds
                })()}
                </tbody>
              </table>
            </div>

            <div>Your regret ratio is: {this.global_local_regret['regretRatio']}</div>

          </div>) : ''}
        <br />
        <button type="button" className="btn btn-primary" onClick={() => window.location.reload()}>
          Return to Welcome
        </button>
        <br />
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
  mode,
  K
}) => ({
  labels,
  candidates,
  mask,
  attributes,
  numLeftPoints,
  mode,
  K
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
