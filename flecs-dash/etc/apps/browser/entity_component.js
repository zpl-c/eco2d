
Vue.component('entity-component', {
  props: [ "name", "value" ],
  methods: {
    remove_component() {
      this.$emit('remove_component', {component: this.name});
    }
  },
  template: `
    <div class="entity-component">
      <div class="entity-component-name">{{ name }}</div>
      <img src="images/delete.png" class="entity-remove-icon" v-on:click="remove_component">

      <component-properties v-for="(v, name) in value" class="component" :key="name"
        :name="name"
        :value="v">
      </component-properties>
    </div>
    `
});
