<template>
  <div id="app">
    <canvas id="canvas" width="100%" height="100%"></canvas>
    <b-navbar toggleable="md" variant="info" prop='fixed'>
      <b-navbar-toggle target="nav_collapse"></b-navbar-toggle>
      <b-navbar-brand href="#">Differential privacy</b-navbar-brand>
      <b-collapse is-nav id="nav_collapse">
        <b-navbar-nav class="ml-auto">
          <b-pagination-nav :link-gen="linkGen"
                            :page-gen="pageGen"
                            :number-of-pages="links.length"
                            v-model="currentPage" />
        </b-navbar-nav>
      </b-collapse>
    </b-navbar>
    <b-container fluid>
      <b-row>
        <b-col cols='3' class='right-line'>
          <b-row v-for="(item,index) in types" :key="index" class='custom-input'>
            <b-col cols="3"><label :for="`input-${item}`">{{ item }}:</label></b-col>
            <b-col cols="9"><b-form-input :id="`input-${item}`" v-model="input_info[item]" :value="input_info[item]" @change="redraw"></b-form-input></b-col>
          </b-row>
          <b-row class='custom-input'>
            <b-col cols="3"><label :for="`input-Epsilon`">Deviation:</label></b-col>
            <b-col cols="9"><b-form-input :id="`input-Epsilon`" v-model="input_info['Epsilon']" :value="input_info['Epsilon']" :type="'range'" min=1 max=20 step=1></b-form-input></b-col>
          </b-row>
          <b-row class='custom-input'>
            <p>{{ input_info['Epsilon'] }}%</p>
          </b-row>
          <b-button v-on:click="submit" size='lg' variant='success' class='custom-input'>Submit</b-button>
          <div class='bottom-left'>
            <h3>Real Answer: {{ realAns }}</h3>
            <h3>Noisy Answer: {{ noisyAns }}</h3>
          </div>
        </b-col>
        <b-col cols='9' id='img_view'>
          <div class="img-container">
            <b-img v-on:click="draw" :src="imgLink" fluid alt="Responsive image" />
          </div>
        </b-col>
      </b-row>
    </b-container>
  </div>
</template>

<script>
export default {
  name: 'app',
  computed: {
    pageLink() {
      return this.linkGen(this.currentPage)
    },
    imgLink() {
      return `./src/assets/${this.img_path[this.currentPage]}.png`
    }
  },
  data () {
    return {
      input_info: {},
      currentPage: 1,
      links: ['#Beijing','#New York City','#Gowalla','#Road'],      
      img_path: ['logo','beijing','nyc','gowalla','road'],
      img_scale: [{},{
        xmin: 39.689237,
        xmax: 40.118373,
        ymin: 116.101327,
        ymax: 116.71854
      },{
        xmin: -74.1,
        xmax: -73.7,
        ymin: 40.5,
        ymax: 41.0
      },
      {
        xmin: -45.9126961833,
        xmax: 78.21870034,
        ymin: -159.765195167,
        ymax: 176.25390279
      },
      {
        xmin: -124.819251,
        xmax: -103.00197,
        ymin: 31.332172,
        ymax: 49.002493
      }],
      types: ['xmin','ymin','xmax','ymax'],
      clickStyle : 0,
      firstClick: {x: 0, y: 0},
      realAns: 0,
      noisyAns: 0,
      saveLife: {
        width:0,
        height: 0,
        clientX: 0,
        clientY: 0
      }
    }
  },
  methods: {
    linkGen (pageNum) {
      return this.links[pageNum - 1]
    },

    pageGen (pageNum) {
      return this.links[pageNum - 1].slice(1)
    },

    draw (event) {
      let canvas = document.getElementById("canvas");
      let ctx = canvas.getContext('2d');
      canvas.width = window.innerWidth;
      canvas.height = window.innerHeight;
      let tmp = this.img_scale[this.currentPage];
      let tmp_x = tmp.xmin + (tmp.xmax - tmp.xmin) * (event.offsetX / event.path[0].width);
      let tmp_y = tmp.ymin + (tmp.ymax - tmp.ymin) * (event.offsetY / event.path[0].height);
      this.saveLife.width = event.path[0].width;
      this.saveLife.height = event.path[0].height;
      this.saveLife.clientX = event.clientX;
      this.saveLife.clientY = event.clientY;
      console.log(event.clientX, event.clientY);
      if (this.clickStyle == 0) {
        console.log("first click");
        this.firstClick.x = event.clientX;
        this.firstClick.y = event.clientY;
        this.input_info.xmin = tmp_x;
        this.input_info.ymin = tmp_y;
      }
      else {
        console.log("second click");
        ctx.beginPath();
        ctx.rect(this.firstClick.x, this.firstClick.y, event.clientX - this.firstClick.x, event.clientY - this.firstClick.y);
        ctx.lineWidth = 7;
        ctx.strokeStyle = "green";
        ctx.stroke();
        this.input_info.xmax = tmp_x;
        this.input_info.ymax = tmp_y;
      }
      this.clickStyle = 1 - this.clickStyle;
    },

    submit () {
      let host = 'http://localhost:8888/';
      let currentName = this.links[this.currentPage - 1].slice(1);
      let url = host + `get_ans?location=${currentName}&xmin=${this.input_info.xmin}&ymin=${this.input_info.ymin}&xmax=${this.input_info.xmax}&ymax=${this.input_info.ymax}&epsilon=${1.0/this.input_info.Epsilon}`;
      fetch(url)
      .then(res => res.json())
      .then(res => {
        this.realAns = res.realAns;
        this.noisyAns = res.noisyAns;
      });
    },

    redraw () {
      console.log("redraw");
      let canvas = document.getElementById("canvas");
      canvas.width = window.innerWidth;
      canvas.height = window.innerHeight;
      let tmp = this.img_scale[this.currentPage];
      let relocation = {
        xmin : 0,
        ymin : 0,
        xmax : 0,
        ymax : 0
      };
      relocation.xmin = (this.input_info.xmin - tmp.xmin) / (tmp.xmax - tmp.xmin) * this.saveLife.width;
      relocation.ymin = (this.input_info.ymin - tmp.ymin) / (tmp.ymax - tmp.ymin) * this.saveLife.height;
      relocation.xmax = (this.input_info.xmax - tmp.xmin) / (tmp.xmax - tmp.xmin) * this.saveLife.width;
      relocation.ymax = (this.input_info.ymax - tmp.ymin) / (tmp.ymax - tmp.ymin) * this.saveLife.height;
      console.log(relocation);
      let ctx = canvas.getContext('2d');
      ctx.beginPath();
      ctx.rect(this.saveLife.clientX + relocation.xmin, this.saveLife.clientY + relocation.ymin, relocation.xmax - relocation.xmin, relocation.ymax - relocation.ymin);
      ctx.lineWidth = 7;
      ctx.strokeStyle = "green";
      ctx.stroke();
    }
  }
}
</script>

<style>
html, body, #app, .container-fluid, #img_view {
  height: 100%;
}

canvas {
  position: absolute;
  z-index: 10;
  top: 0px;
  left: 0px;
  background: rgba(255, 255, 255, 0);
  pointer-events:none
}

.img-container {
  margin-top: 15px;
  margin-bottom: 15px;
}

#app {
  font-family: 'Avenir', Helvetica, Arial, sans-serif;
  text-align: center;
}

.right-line {
  border-right: solid 1px;
}

.custom-input {
  margin-top: 15px;
  margin-bottom: 15px;
}

.bottom-left {
  text-align: left;
  bottom: 20px;
  position: absolute;
}
</style>
