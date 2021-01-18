

Vue.component('system', {
  props: ['tick', 'system', 'system_time'],
  methods: {
    entity_count() {
      return this.system.current.matched_entity_count;
    },
    table_count() {
      return this.system.current.matched_table_count;
    },
    fragmentation() {
      return (((this.table_count() - 1) / this.entity_count()) * 100).toFixed(1) + "%";
    },
    percentage() {
      return (this.system.current.time_spent / this.system_time) * 100;
    },
    percentage_class() {
      let pct = this.percentage();
      if (pct < 1.0) {
        return "system-lowest";
      } else if (pct < 5.0) {
        return "system-low";
      } else if (pct < 10.0) {
        return "system-medium";
      } else if (pct < 20.0) {
        return "system-high";
      } else {
        return "system-highest";
      }
    }    
  },
  template: `
    <div>
      <div :class="'system ' + percentage_class()">
        <div class="system-header">
          {{system.name}}
          <span class="system-module" v-if="system.module">{{system.module}}</span>
          <span class="system-metrics" v-if="entity_count()">T:{{table_count()}} - F:{{fragmentation()}}</span>
          <system-time-graph :system="system" :system_time="system_time" :percentage="percentage()" :tick="tick"></system-time-graph>
        </div> 
      </div>
      <div :class="'system-query ' + percentage_class()">
        <system-component v-for="(component, i) in system.signature" :component="component" :key="i"></system-component>
        <system-entity-graph :system="system" :tick="tick"></system-entity-graph>
      </div>
    </div>`
});
