import React from "react";
import THREE from "./three";

const SCALE = 100;
const FACTOR = 10; // font size = SCALE / FACTOR

// the convex hull of the preference space
class ConvexHull extends React.Component {
  componentDidMount() {
    const width = this.mount.clientWidth;
    const height = this.mount.clientHeight;

    this.scene = new THREE.Scene();

    this.camera = new THREE.PerspectiveCamera(75, width / height, 0.1, 1000);
    this.camera.position.set(1.5 * SCALE, 1.5 * SCALE, 0.5 * SCALE);
    this.camera.up.set(0, 0, 1);
    this.controls = new THREE.OrbitControls(this.camera, this.mount);
    this.controls.update();

    this.renderer = new THREE.WebGLRenderer({ antialias: true });
    this.renderer.setClearColor(0xffffff);
    this.renderer.setSize(width, height);
    this.mount.appendChild(this.renderer.domElement);
    this.addAxes();
    this.drawGeometry();
    this.start();
  }

  drawGeometry = () => {
    let newVers = this.props.vertices.map(x => x);
    // if (newVers.length < 4) return;
    if (newVers.length < 4) {
      if (newVers.length == 3) {
        const tmp = newVers.at(0);
        newVers.push(tmp);
      } else if (newVers.length == 2) {
        const tmp = newVers.at(0);
        const tmp1 = newVers.at(1);
        newVers.push(tmp);
        newVers.push(tmp1);
      } else {
        return;
      }
    }
    // if (this.props.vertices.length < 4) {
    //   if (this.props.vertices.length == 3) {
    //     console.log('333-1', this.props.vertices);

    //     let ver = this.props.vertices.at(0);
    //     this.props.vertices.push(ver);
    //     console.log('333-2', this.props.vertices);
    //   }
    //   if (this.props.vertices.length == 2) {
    //     console.log('222-1', this.props.vertices);

    //     let ver = this.props.vertices.at(0);
    //     this.props.vertices.push(ver);
    //     this.props.vertices.push(ver);
    //     console.log('222-2', this.props.vertices);
    //   } else {
    //       return;
    //   }
    // }
    // if (this.props.vertices.length < 4) return;
    const points = newVers.map(vertex => {
      const scaled_vertex = vertex.map(x => x * SCALE);
      return new THREE.Vector3(...scaled_vertex);
    });
    console.log(points, 'points to paint');
    const geometry = new THREE.ConvexGeometry(points);
    const material = new THREE.MeshBasicMaterial({
      color: 0xff0000,
      //shading: THREE.FlatShading,
      polygonOffset: true,
      polygonOffsetFactor: 1, // positive value pushes polygon further away
      polygonOffsetUnits: 1,
      opacity: 0.5,
      transparent: true
    });
    this.mesh = new THREE.Mesh(geometry, material);
    const geo = new THREE.EdgesGeometry(geometry); // or WireframeGeometry
    const mat = new THREE.LineBasicMaterial({ color: 0x000000, linewidth: 2 });
    const wireframe = new THREE.LineSegments(geo, mat);
    this.mesh.add(wireframe);
    this.scene.add(this.mesh);
  };

  addAxes = () => {
    const axes = new THREE.AxesHelper(1.5 * SCALE);
    this.scene.add(axes);
    const typeface = require("three/examples/fonts/helvetiker_regular.typeface.json");
    const font = new THREE.Font(typeface);

    const numConfig = [
      // color, position, rotation
      [
        0xff0000,
        [SCALE + SCALE / (FACTOR * 2), 0, 0],
        [Math.PI / 2, Math.PI, 0]
      ],
      [
        0x00ff00,
        [0, SCALE - SCALE / (FACTOR * 2), 0],
        [Math.PI / 2, Math.PI / 2, 0]
      ],
      [
        0x0000ff,
        [
          SCALE / (FACTOR * Math.sqrt(2)),
          -SCALE / (FACTOR * Math.sqrt(2)),
          SCALE
        ],
        [Math.PI / 2, (Math.PI * 3) / 4, 0]
      ]
    ];

    numConfig.forEach(([color, position, rotation]) => {
      const shape = font.generateShapes("1", SCALE / FACTOR);
      const geometry = new THREE.ShapeBufferGeometry(shape);
      const mesh = new THREE.Mesh(
        geometry,
        new THREE.MeshBasicMaterial({
          color: color,
          side: THREE.DoubleSide
        })
      );
      mesh.position.set(...position);
      mesh.rotation.set(...rotation);
      this.scene.add(mesh);
    });
    window.lines = [];
    const lineConfig = [
      [0xff0000, [SCALE, 0, 0], [0, 0, 0]],
      [0x00ff00, [0, SCALE, 0], [0, 0, -Math.PI / 2]],
      [0x0000ff, [0, 0, SCALE], [0, 0, -Math.PI / 2]]
    ];
    lineConfig.forEach(([color, position, rotation]) => {
      const material = new THREE.LineBasicMaterial({
        color
      });
      const geometry = new THREE.Geometry();
      geometry.vertices.push(
        new THREE.Vector3(0, 0, 0),
        new THREE.Vector3(0, SCALE / (2 * FACTOR))
      );
      const line = new THREE.Line(geometry, material);
      line.position.set(...position);
      line.rotation.set(...rotation);
      this.scene.add(line);
      window.lines.push(line);
    });
  };

  componentDidUpdate(prevProps) {
    if (this.props.vertices !== prevProps.vertices) {
      if (this.mesh) {
        this.scene.remove(this.mesh);
        this.mesh.geometry.dispose();
        this.mesh.material.dispose();
      }
      this.drawGeometry();
    }
  }

  componentWillUnmount() {
    this.stop();
    this.mount.removeChild(this.renderer.domElement);
  }

  start = () => {
    if (!this.frameId) {
      this.frameId = window.requestAnimationFrame(this.animate);
    }
  };

  stop = () => {
    window.cancelAnimationFrame(this.frameId);
  };

  animate = () => {
    this.controls.update();
    this.renderer.render(this.scene, this.camera);
    this.frameId = window.requestAnimationFrame(this.animate);
  };

  render() {
    return (
      <div
        style={{ width: "25rem", height: "25rem" }}
        ref={mount => {
          this.mount = mount;
        }}
      />
    );
  }
}

export default ConvexHull;
