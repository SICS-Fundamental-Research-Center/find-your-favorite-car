import React from "react";
import { connect } from "react-redux";
import {
  FlexibleXYPlot,
  XAxis,
  YAxis,
  Hint,
  VerticalBarSeries,
  ChartLabel
} from "react-vis";

// a histogram to show the number of tuples left vs the number of questions asked.
class Histogram extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      hintValue: null
    };
  }

  _onMouseLeave = () => {
    this.setState({
      hintValue: null
    });
  };

  _onNearestX = (value, { index }) => {
    this.setState({
      hintValue: this.props.data[index]
    });
  };

  render() {
    // console.log("data??", this.props.data, this.props.leftPoints);
    return (
      <div className="row justify-content-center" style={{ width: "25rem"}}>
        <h4>No. of Tuples Left vs. No. of Questions Asked</h4>
        <div style={{ width: "25rem", height: "25rem" }}>
          <FlexibleXYPlot onMouseLeave={this._onMouseLeave}>
            <XAxis
              tickValues={this.props.data.map((val, idx) => idx)}
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
              text="Question No."
              includeMargin={false}
              xPercent={0.82}
              yPercent={1.06}
              style={{
                style: { fill: "black" }
              }}
            />
            <ChartLabel
              text="No. of Tuples Left"
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
              onNearestX={this._onNearestX}
              data={this.props.data}
              animation
            />
            {this.state.hintValue && (
              <Hint value={this.state.hintValue}>
                <p style={{ color: "black" }}>
                  Q{this.state.hintValue.x}: {this.state.hintValue.y} Tuples Left
                </p>
              </Hint>
            )}
          </FlexibleXYPlot>
        </div>
      </div>
    );
  }
}

const mapStateToProps = ({ numLeftPoints, leftPoints }) => ({
  data: numLeftPoints.map((num, i) => ({ x: i, y: num })),
  leftPoints
});

export default connect(mapStateToProps)(Histogram);
