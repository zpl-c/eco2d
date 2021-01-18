
Vue.component('entity-tag', {
    props: [ "name" ],
    methods: {
      remove_component() {
        this.$emit('remove_component', {component: this.name});
      }
    },
    template: `
      <span class="entity-tag">
        {{ name }}
        <img src="images/close.png" class="entity-remove-icon" v-on:click="remove_component">
      </span>
      `
  });
  